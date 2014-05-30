//------------------------------------------------------------------------------
//  slesaudiorenderer.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "slesaudiorenderer.h"
#include <stdio.h>
#include <math.h>

namespace chrissly
{

SLESAudioRenderer* SLESAudioRenderer::Singleton = NULL;

SLEnvironmentalReverbSettings SLESAudioRenderer::reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;

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
SLESAudioRenderer::_Initialise(void* customParams)
{
    SLresult result = slCreateEngine(&this->engine, 0, NULL, 0, NULL, NULL);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::_Initialise(): failed to create engine\n");

    result = (*this->engine)->Realize(this->engine, SL_BOOLEAN_FALSE);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::_Initialise(): failed to realize engine\n");

    result = (*this->engine)->GetInterface(this->engine, SL_IID_ENGINE, &this->engineInterface);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::_Initialise(): failed to get engine interface\n");

    const SLInterfaceID  ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean required[1] = {SL_BOOLEAN_TRUE};
    result = (*this->engineInterface)->CreateOutputMix(this->engineInterface, &this->outputMix, 1, ids, required);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::_Initialise(): failed to create outputmix\n");

    result = (*this->outputMix)->Realize(this->outputMix, SL_BOOLEAN_FALSE);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::_Initialise(): failed to realize outputmix\n");

    result = (*this->outputMix)->GetInterface(this->outputMix, SL_IID_ENVIRONMENTALREVERB, &this->outputMixEnvironmentalReverb);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::_Initialise(): failed to get environmental reverb interface\n");

    result = (*this->outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(this->outputMixEnvironmentalReverb, &reverbSettings);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::_Initialise(): failed to set environmental reverb properties\n");
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
unsigned short
SLESAudioRenderer::GetNumHardwareChannels() const
{
    return (unsigned short)this->numHardwareChannels;
}

//------------------------------------------------------------------------------
/**
*/
void
SLESAudioRenderer::StartChannel(audio::Channel* channel)
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

    SLAndroidSimpleBufferQueueItf bufferQueueInterface = channel->GetBufferQueueInterface();
    result = (*bufferQueueInterface)->Enqueue(bufferQueueInterface, sound->_GetSampleBufferPointer(0), length * numChannels * (bits >> 3));
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::StartChannel(): failed to enqueue buffer\n");

    channel->_SetIndex(0);
    channel->_SetIsPlaying(true);

    SLEffectSendItf effectSend = channel->GetEffectSendInterface();
    result = (*effectSend)->EnableEffectSend(effectSend, this->outputMixEnvironmentalReverb, SL_BOOLEAN_TRUE, 0);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::StartChannel(): failed to enable effect send\n");
}

//------------------------------------------------------------------------------
/**
*/
void
SLESAudioRenderer::UpdateChannel(audio::Channel* channel)
{
    bool isPlaying;
    channel->IsPlaying(&isPlaying);

    if (isPlaying)
    {
        if (channel->_PropertiesHasChanged())
        {
            SLresult result;
            float volume;
            channel->GetVolume(&volume);
            SLVolumeItf volumeInterface = channel->GetVolumeInterface();
            SLmillibel slVolume = SL_MILLIBEL_MIN;
            if (volume >= 1.0f)
            {
                result = (*volumeInterface)->GetMaxVolumeLevel(volumeInterface, &slVolume);
                CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::UpdateChannel(): failed to get max volume level\n");
            }
            else if (volume <= 0.0f)
            {
                slVolume = SL_MILLIBEL_MIN;
            }
            else
            {
                slVolume = M_LN2 / log(1.0f / (1.0f - volume)) * -1000.0f;
            }
            result = (*volumeInterface)->SetVolumeLevel(volumeInterface, slVolume);
            CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::UpdateChannel(): failed to set volume\n");

            float pan;
            channel->GetPan(&pan);
            result = (*volumeInterface)->EnableStereoPosition(volumeInterface, SL_BOOLEAN_TRUE);
            CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::UpdateChannel(): failed to enable stereo position\n");
            result = (*volumeInterface)->SetStereoPosition(volumeInterface, (SLpermille)(pan * 1000.0f));
            CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::UpdateChannel(): failed to set stereo position\n");

            bool paused;
            channel->GetPaused(&paused);
            SLPlayItf playerInterface = channel->GetPlayerInterface();
            result = (*playerInterface)->SetPlayState(playerInterface, paused ? SL_PLAYSTATE_PAUSED : SL_PLAYSTATE_PLAYING);
            CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESAudioRenderer::UpdateChannel(): failed to set play state\n");
        }
    }
    else
    {
        channel->Release();
        audio::Mode mode;
        channel->GetMode(&mode);
        if (mode & audio::MODE_LOOP_NORMAL)
        {
            this->StartChannel(channel);
        }
        else
        {
            channel->_SetIndex(audio::Channel::CHANNEL_FREE);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SLESAudioRenderer::ReleaseChannel(audio::Channel* channel)
{
    channel->Release();
    channel->_SetIsPlaying(false);
}

//------------------------------------------------------------------------------
/**
*/
void
SLESAudioRenderer::BufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueueInterface, void* context)
{
    audio::Channel* channel = (audio::Channel*)context;
    channel->_SetIsPlaying(false);
}

} // namespace chrissly
