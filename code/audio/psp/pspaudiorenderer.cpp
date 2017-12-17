//------------------------------------------------------------------------------
//  pspaudiorenderer.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "pspaudiorenderer.h"
#include "modeflags.h"
#include "sound.h"
#include "propertychangedflags.h"
#include "dsp/mixer.h"
#include "debug.h"
#include <stdio.h>
#include <pspthreadman.h>

namespace chrissly
{

PSPAudioRenderer* PSPAudioRenderer::Singleton = NULL;

static const int NumOutputSamples = 1024;

//------------------------------------------------------------------------------
/**
*/
PSPAudioRenderer::PSPAudioRenderer()
{
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
PSPAudioRenderer::~PSPAudioRenderer()
{
    Singleton = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
PSPAudioRenderer::Initialise(void* const customParams)
{

}

//------------------------------------------------------------------------------
/**
*/
void
PSPAudioRenderer::Shutdown()
{

}

//------------------------------------------------------------------------------
/**
*/
void
PSPAudioRenderer::StartAudioProcessing()
{

}

//------------------------------------------------------------------------------
/**
*/
void
PSPAudioRenderer::StopAudioProcessing()
{

}

//------------------------------------------------------------------------------
/**
*/
unsigned short
PSPAudioRenderer::GetNumHardwareChannels() const
{
    return PSP_AUDIO_CHANNEL_MAX;
}

//------------------------------------------------------------------------------
/**
*/
void
PSPAudioRenderer::StartChannel(audio::Channel* const channel)
{
    const core::Mutex& syncLock = channel->_GetSyncLock();
    syncLock.Lock();

    audio::Sound* sound;
    channel->GetCurrentSound(&sound);
    int index, numChannels;
    sound->GetFormat(NULL, NULL, &numChannels, NULL);

    PspAudioFormats format = PSP_AUDIO_FORMAT_MONO;
    switch (numChannels)
    {
        case 1:
            format = PSP_AUDIO_FORMAT_MONO;
            break;
        case 2:
            format = PSP_AUDIO_FORMAT_STEREO;
            break;
        default:
            CE_ASSERT(false, "PSPAudioRenderer::StartChannel(): %i channel audio output not supported\n", numChannels);
    }

    channel->GetIndex(&index);
    index = sceAudioChReserve(index, PSP_AUDIO_SAMPLE_ALIGN(NumOutputSamples), format);
    channel->_SetIndex(index);
    if (index != audio::Channel::CHANNEL_FREE)
    {
        channel->_SetIsPlaying(true);
        sound->_IncrementUseCount();
    }

    syncLock.Unlock();
}

//------------------------------------------------------------------------------
/**
*/
void
PSPAudioRenderer::UpdateChannel(audio::Channel* const channel)
{

}

//------------------------------------------------------------------------------
/**
*/
void
PSPAudioRenderer::ReleaseChannel(audio::Channel* const channel)
{
    const core::Mutex& syncLock = channel->_GetSyncLock();
    syncLock.Lock();

    channel->RequestRelease();

    syncLock.Unlock();
}

//------------------------------------------------------------------------------
/**
*/
int
PSPAudioRenderer::ChannelThread(SceSize args, void* argp)
{
    audio::Channel* channel = (audio::Channel*)(*(uintptr_t*)argp);
    const core::Mutex& syncLock = channel->_GetSyncLock();
    while (true)
    {
        syncLock.Lock();

        int index;
        channel->GetIndex(&index);
        audio::Sound* sound;
        channel->GetCurrentSound(&sound);

        if (channel->GetReleaseRequest())
        {
            sceAudioChangeChannelVolume(index, 0, 0);
            sceAudioChRelease(index);
            channel->_SetIsPlaying(false);
            channel->_SetIndex(audio::Channel::CHANNEL_FREE);
            sound->_DecrementUseCount();
        }

        bool isPlaying, paused;
        channel->IsPlaying(&isPlaying);
        channel->GetPaused(&paused);
        if (!isPlaying || paused)
        {
            syncLock.Unlock();
            sceKernelDelayThread(10000U);
            continue;
        }

        unsigned int length, position;
        sound->GetLength(&length);
        channel->GetPosition(&position);
        audio::Mode mode;
        channel->GetMode(&mode);

        int samplesRemaining = length - position;
        if (samplesRemaining > 0)
        {
            unsigned int sampleCount = NumOutputSamples;
            if ((unsigned int)samplesRemaining <= sampleCount)
            {
                sampleCount = PSP_AUDIO_SAMPLE_ALIGN(samplesRemaining);
                sceAudioSetChannelDataLen(index, sampleCount > PSP_AUDIO_SAMPLE_MIN ? sampleCount - PSP_AUDIO_SAMPLE_MIN : 0);
            }
            channel->SetPosition(position + sampleCount);

            int numChannels;
            sound->GetFormat(NULL, NULL, &numChannels, NULL);
            float volume, pan;
            channel->GetVolume(&volume);
            channel->GetPan(&pan);
            float leftVolume, rightVolume;
            if (mode & audio::MODE_3D)
            {
                ce_audio_calculate_stereo_channel_volumes(PAN_CONSTANTPOWER, volume *= channel->_GetAttenuationFactor(), pan, &leftVolume, &rightVolume);
            }
            else if (1 == numChannels)
            {
                ce_audio_calculate_stereo_channel_volumes(PAN_CONSTANTPOWER, volume, pan, &leftVolume, &rightVolume);
            }
            else
            {
                ce_audio_calculate_stereo_channel_volumes(PAN_STEREO, volume, pan, &leftVolume, &rightVolume);
            }

            void* sampleBuffer = mode & audio::MODE_CREATESTREAM ? sound->_GetCodec()->FillStreamBuffer(sampleCount, position) : sound->_GetSampleBufferPointer(position);

            syncLock.Unlock();

            sceAudioOutputPannedBlocking(index, (int)((float)PSP_AUDIO_VOLUME_MAX * leftVolume), (int)((float)PSP_AUDIO_VOLUME_MAX * rightVolume), sampleBuffer);
        }
        else
        {
            if (mode & audio::MODE_LOOP_NORMAL)
            {
                sceAudioSetChannelDataLen(index, PSP_AUDIO_SAMPLE_ALIGN(NumOutputSamples));
                channel->SetPosition(0U);
            }
            else
            {
                sceAudioChRelease(index);
                channel->_SetIsPlaying(false);
                channel->_SetIndex(audio::Channel::CHANNEL_FREE);
                sound->_DecrementUseCount();
            }

            syncLock.Unlock();
        }
    }

    return 0;
}

} // namespace chrissly