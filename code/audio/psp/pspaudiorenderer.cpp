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
PSPAudioRenderer::_Initialise(void* customParams)
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
PSPAudioRenderer::StartChannel(audio::Channel* channel)
{
    const core::Mutex& syncLock = channel->_GetSyncLock();
    syncLock.Lock();

    int index;
    channel->GetIndex(&index);
    audio::Sound* sound;
    channel->GetCurrentSound(&sound);
    unsigned int length;
    sound->GetLength(&length);
    int numChannels, bits;
    sound->GetFormat(NULL, NULL, &numChannels, &bits);
    audio::Mode mode;
    channel->GetMode(&mode);

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

    unsigned int samplecount = 0U;
    if (mode & audio::MODE_CREATESTREAM)
    {
        audio::Codec* codec = sound->_GetCodec();
        codec->InitialiseStream();
        samplecount = codec->GetStreamBufferLength() / (numChannels * ((unsigned int)bits >> 3U));
    }
    else
    {
        samplecount = NumOutputSamples;
    }

    index = sceAudioChReserve(index, PSP_AUDIO_SAMPLE_ALIGN(samplecount), format);
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
PSPAudioRenderer::UpdateChannel(audio::Channel* channel)
{

}

//------------------------------------------------------------------------------
/**
*/
void
PSPAudioRenderer::ReleaseChannel(audio::Channel* channel)
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

        bool paused;
        channel->GetPaused(&paused);
        if (paused)
        {
            syncLock.Unlock();
            sceKernelDelayThread(10000U);
            continue;
        }

        bool isPlaying;
        channel->IsPlaying(&isPlaying);
        if (isPlaying)
        {
            unsigned int length;
            sound->GetLength(&length);
            unsigned int position;
            channel->GetPosition(&position);
            audio::Mode mode;
            channel->GetMode(&mode);
            int numChannels, bits;
            sound->GetFormat(NULL, NULL, &numChannels, &bits);
            audio::Codec* codec = sound->_GetCodec();
            unsigned int samplecount = 0U;

            int samplesRemaining = length - position;
            if (samplesRemaining > 0)
            {
                if (mode & audio::MODE_CREATESTREAM)
                {
                    // dont swap buffers on the first frame
                    if (position > 0)
                    {
                        codec->FillStreamBackBuffer();
                        codec->SwapStreamBuffers();
                    }
                    samplecount = codec->GetStreamBufferLength() / (numChannels * ((unsigned int)bits >> 3U));
                }
                else
                {
                    samplecount = NumOutputSamples;
                }
                if ((unsigned int)samplesRemaining <= samplecount)
                {
                    samplecount = PSP_AUDIO_SAMPLE_ALIGN(samplesRemaining);
                    sceAudioSetChannelDataLen(index, samplecount > PSP_AUDIO_SAMPLE_MIN ? samplecount - PSP_AUDIO_SAMPLE_MIN : 0);
                }
                channel->SetPosition(position + samplecount);

                float volume;
                channel->GetVolume(&volume);
                float pan;
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

                syncLock.Unlock();

                if (mode & audio::MODE_CREATESTREAM)
                {
                    sceAudioOutputPannedBlocking(index, (int)((float)PSP_AUDIO_VOLUME_MAX * leftVolume), (int)((float)PSP_AUDIO_VOLUME_MAX * rightVolume), codec->GetStreamBufferPointer());
                }
                else
                {
                    sceAudioOutputPannedBlocking(index, (int)((float)PSP_AUDIO_VOLUME_MAX * leftVolume), (int)((float)PSP_AUDIO_VOLUME_MAX * rightVolume), sound->_GetSampleBufferPointer(position));
                }
            }
            else
            {
                if (mode & audio::MODE_LOOP_NORMAL)
                {
                    if (mode & audio::MODE_CREATESTREAM)
                    {
                        codec->InitialiseStream();
                        samplecount = codec->GetStreamBufferLength() / (numChannels * ((unsigned int)bits >> 3U));
                    }
                    else
                    {
                        samplecount = NumOutputSamples;
                    }
                    sceAudioSetChannelDataLen(index, PSP_AUDIO_SAMPLE_ALIGN(samplecount));
                    channel->SetPosition(0U);

                    syncLock.Unlock();
                }
                else
                {
                    sceAudioChRelease(index);
                    channel->_SetIsPlaying(false);
                    channel->_SetIndex(audio::Channel::CHANNEL_FREE);
                    sound->_DecrementUseCount();

                    syncLock.Unlock();
                }
            }
        }
        else
        {
            syncLock.Unlock();
            sceKernelDelayThread(10000U);
        }
    }

    return 0;
}

} // namespace chrissly