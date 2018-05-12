//------------------------------------------------------------------------------
//  slesaudiorenderer.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "slesaudiorenderer.h"
#include "codec.h"
#include "chrisslymutex.h"
#include <stdio.h>
#include <math.h>

namespace chrissly
{

SLESAudioRenderer* SLESAudioRenderer::Singleton = NULL;

static const SLEnvironmentalReverbSettings ReverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;

//------------------------------------------------------------------------------
/**
*/
SLESAudioRenderer::SLESAudioRenderer() :
    numHardwareChannels(8),
    engine(NULL),
    engineInterface(NULL),
    outputMix(NULL),
    outputMixEnvironmentalReverb(NULL)
{
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
SLESAudioRenderer::~SLESAudioRenderer()
{
    Singleton = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
SLESAudioRenderer::Initialise(void* const customParams)
{
    SLresult result = slCreateEngine(&this->engine, 0U, NULL, 0U, NULL, NULL);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::Initialise(): failed to create engine\n");

    result = (*this->engine)->Realize(this->engine, SL_BOOLEAN_FALSE);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::Initialise(): failed to realize engine\n");

    result = (*this->engine)->GetInterface(this->engine, SL_IID_ENGINE, &this->engineInterface);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::Initialise(): failed to get engine interface\n");

    const SLInterfaceID  ids[1U] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean required[1U] = {SL_BOOLEAN_TRUE};
    result = (*this->engineInterface)->CreateOutputMix(this->engineInterface, &this->outputMix, 1U, ids, required);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::Initialise(): failed to create outputmix\n");

    result = (*this->outputMix)->Realize(this->outputMix, SL_BOOLEAN_FALSE);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::Initialise(): failed to realize outputmix\n");

    result = (*this->outputMix)->GetInterface(this->outputMix, SL_IID_ENVIRONMENTALREVERB, &this->outputMixEnvironmentalReverb);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::Initialise(): failed to get environmental reverb interface\n");

    result = (*this->outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(this->outputMixEnvironmentalReverb, &ReverbSettings);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::Initialise(): failed to set environmental reverb properties\n");
}

//------------------------------------------------------------------------------
/**
*/
void
SLESAudioRenderer::Shutdown()
{
    if (this->outputMix != NULL)
    {
        (*this->outputMix)->Destroy(this->outputMix);
        this->outputMix = NULL;
    }

    if (this->engine != NULL)
    {
        (*this->engine)->Destroy(this->engine);
        this->engine = NULL;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SLESAudioRenderer::StartAudioProcessing()
{

}

//------------------------------------------------------------------------------
/**
*/
void
SLESAudioRenderer::StopAudioProcessing()
{

}

//------------------------------------------------------------------------------
/**
*/
unsigned short
SLESAudioRenderer::GetNumHardwareChannels() const
{
    return (unsigned short)this->numHardwareChannels;
}

//------------------------------------------------------------------------------
/**
*/
void
SLESAudioRenderer::StartChannel(audio::Channel* const channel)
{
    channel->SetupAudioPlayer(this->engineInterface, this->outputMix);

    bool paused;
    channel->GetPaused(&paused);
    SLPlayItf playerInterface = channel->GetPlayerInterface();
    SLresult result = (*playerInterface)->SetPlayState(playerInterface, paused ? SL_PLAYSTATE_PAUSED : SL_PLAYSTATE_PLAYING);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::StartChannel(): failed to set play state\n");

    audio::Sound* sound;
    channel->GetCurrentSound(&sound);
    unsigned int length;
    sound->GetLength(&length);
    int numChannels, bits;
    sound->GetFormat(NULL, NULL, &numChannels, &bits);
    audio::Mode mode;
    channel->GetMode(&mode);

    sound->IncrementUseCount();
    SLAndroidSimpleBufferQueueItf bufferQueueInterface = channel->GetBufferQueueInterface();
    if (mode & audio::MODE_CREATESTREAM)
    {
        audio::Codec* codec = sound->GetCodec();
        codec->InitialiseStream();
        result = (*bufferQueueInterface)->Enqueue(bufferQueueInterface, codec->GetStreamBufferPointer(), codec->GetStreamBufferLength());
    }
    else
    {
        result = (*bufferQueueInterface)->Enqueue(bufferQueueInterface, sound->GetSampleBufferPointer(0U), length * numChannels * ((unsigned int)bits >> 3U));
    }
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::StartChannel(): failed to enqueue buffer\n");

    channel->SetIndex(0);
    channel->SetIsPlaying(true);

    SLEffectSendItf effectSend = channel->GetEffectSendInterface();
    result = (*effectSend)->EnableEffectSend(effectSend, this->outputMixEnvironmentalReverb, SL_BOOLEAN_FALSE, 0);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::StartChannel(): failed to enable effect send\n");
}

//------------------------------------------------------------------------------
/**
*/
void
SLESAudioRenderer::UpdateChannel(audio::Channel* const channel)
{
    audio::Mode mode;
    channel->GetMode(&mode);
    if (mode & audio::MODE_CREATESTREAM)
    {
        const core::Mutex& syncLock = channel->GetSyncLock();
        syncLock.Lock();
        audio::Sound* sound;
        channel->GetCurrentSound(&sound);
        sound->GetCodec()->FillStreamBackBuffer();
        syncLock.Unlock();
    }

    audio::PropertyChange propertyChange = channel->PropertiesHaveChanged();
    if (propertyChange != audio::UNCHANGED)
    {
        SLresult result;
        if (propertyChange & audio::PROPERTY_VOLUME || propertyChange & audio::PROPERTY_ATTENUATION || propertyChange & audio::PROPERTY_MODE)
        {
            float volume;
            channel->GetVolume(&volume);
            if (mode & audio::MODE_3D)
            {
                volume *= channel->GetAttenuationFactor();
            }
            SLVolumeItf volumeInterface = channel->GetVolumeInterface();
            SLmillibel slVolume;
            if (volume >= 1.0f)
            {
                result = (*volumeInterface)->GetMaxVolumeLevel(volumeInterface, &slVolume);
                CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::UpdateChannel(): failed to get max volume level\n");
            }
            else if (volume < 0.020931f)
            {
                slVolume = SL_MILLIBEL_MIN;
            }
            else
            {
                slVolume = (float)M_LN2 / logf(1.0f / (1.0f - volume)) * -1000.0f;
            }
            result = (*volumeInterface)->SetVolumeLevel(volumeInterface, slVolume);
            CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::UpdateChannel(): failed to set volume level to: '%f' (SLmillibel '%i')\n", volume, slVolume);
        }

        if (propertyChange & audio::PROPERTY_PAN)
        {
            float pan;
            channel->GetPan(&pan);
            SLVolumeItf volumeInterface = channel->GetVolumeInterface();
            result = (*volumeInterface)->EnableStereoPosition(volumeInterface, SL_BOOLEAN_TRUE);
            CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::UpdateChannel(): failed to enable stereo position\n");
            result = (*volumeInterface)->SetStereoPosition(volumeInterface, (SLpermille)(pan * 1000.0f));
            CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::UpdateChannel(): failed to set stereo position\n");
        }

        if (propertyChange & audio::PROPERTY_PAUSED)
        {
            bool paused;
            channel->GetPaused(&paused);
            SLPlayItf playerInterface = channel->GetPlayerInterface();
            result = (*playerInterface)->SetPlayState(playerInterface, paused ? SL_PLAYSTATE_PAUSED : SL_PLAYSTATE_PLAYING);
            CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::UpdateChannel(): failed to set play state\n");
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SLESAudioRenderer::ReleaseChannel(audio::Channel* const channel)
{
    channel->ReleaseAudioPlayer();
    channel->ReleaseInternal();
}

//------------------------------------------------------------------------------
/**
*/
void
SLESAudioRenderer::BufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueueInterface, void* context)
{
    audio::Channel* channel = (audio::Channel*)context;
    SLresult result;
    audio::Sound* sound;
    channel->GetCurrentSound(&sound);
    audio::Mode mode;
    channel->GetMode(&mode);
    if (mode & audio::MODE_CREATESTREAM)
    {
        const core::Mutex& syncLock = channel->GetSyncLock();
        syncLock.Lock();
        audio::Codec* codec = sound->GetCodec();
        if (codec->EndOfStream())
        {
            if (mode & audio::MODE_LOOP_NORMAL)
            {
                codec->InitialiseStream();
                result = (*bufferQueueInterface)->Enqueue(bufferQueueInterface, codec->GetStreamBufferPointer(), codec->GetStreamBufferLength());
                CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::BufferQueueCallback(): failed to enqueue buffer for streaming\n");
            }
            else
            {
                channel->ReleaseAudioPlayer();
                channel->ReleaseInternal();
            }
        }
        else
        {
            codec->SwapStreamBuffers();
            result = (*bufferQueueInterface)->Enqueue(bufferQueueInterface, codec->GetStreamBufferPointer(), codec->GetStreamBufferLength());
            CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::BufferQueueCallback(): failed to enqueue buffer for streaming\n");
        }
        syncLock.Unlock();
    }
    else
    {
        if (mode & audio::MODE_LOOP_NORMAL)
        {
            unsigned int length;
            sound->GetLength(&length);
            int numChannels, bits;
            sound->GetFormat(NULL, NULL, &numChannels, &bits);

            result = (*bufferQueueInterface)->Enqueue(bufferQueueInterface, sound->GetSampleBufferPointer(0U), length * numChannels * ((unsigned int)bits >> 3U));
            CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::BufferQueueCallback(): failed to enqueue buffer\n");
        }
        else
        {
            channel->ReleaseAudioPlayer();
            channel->ReleaseInternal();
        }
    }
}

} // namespace chrissly
