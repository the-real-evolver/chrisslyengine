//------------------------------------------------------------------------------
//  pspaudiorenderer.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "pspaudiorenderer.h"
#include "modeflags.h"
#include "sound.h"
#include "propertychangedflags.h"
#include "chrisslymath.h"
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

    unsigned int samplecount = 0;
    if (mode & audio::MODE_CREATESTREAM)
    {
        audio::Codec* codec = sound->_GetCodec();
        codec->InitialiseStream();
        samplecount = codec->GetStreamBufferLength() / (numChannels * (bits >> 3));
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
            sceKernelDelayThread(10000);
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
            unsigned int samplecount = 0;

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
                    samplecount = codec->GetStreamBufferLength() / (numChannels * (bits >> 3));
                }
                else
                {
                    samplecount = NumOutputSamples;
                }
                if ((unsigned int)samplesRemaining < samplecount)
                {
                    samplecount = PSP_AUDIO_SAMPLE_ALIGN(samplesRemaining);
                    sceAudioSetChannelDataLen(index, samplecount > PSP_AUDIO_SAMPLE_MIN ? samplecount - PSP_AUDIO_SAMPLE_MIN : 0);
                }
                channel->SetPosition(position + samplecount);

                float volume;
                channel->GetVolume(&volume);
                float panning;
                channel->GetPan(&panning);
                int leftVolume, rightVolume;
                if (mode & audio::MODE_3D)
                {
                    CalculateVolumesFromPanning(PAN_CONSTANTPOWER, volume *= channel->_GetAttenuationFactor(), panning, leftVolume, rightVolume);
                }
                else if (1 == numChannels)
                {
                    CalculateVolumesFromPanning(PAN_CONSTANTPOWER, volume, panning, leftVolume, rightVolume);
                }
                else
                {
                    CalculateVolumesFromPanning(PAN_CLAMPEDLINEAR, volume, panning, leftVolume, rightVolume);
                }

                syncLock.Unlock();

                if (mode & audio::MODE_CREATESTREAM)
                {
                    sceAudioOutputPannedBlocking(index, leftVolume, rightVolume, codec->GetStreamBufferPointer());
                }
                else
                {
                    sceAudioOutputPannedBlocking(index, leftVolume, rightVolume, sound->_GetSampleBufferPointer(position));
                }
            }
            else
            {
                if (mode & audio::MODE_LOOP_NORMAL)
                {
                    if (mode & audio::MODE_CREATESTREAM)
                    {
                        codec->InitialiseStream();
                        samplecount = codec->GetStreamBufferLength() / (numChannels * (bits >> 3));
                    }
                    else
                    {
                        samplecount = NumOutputSamples;
                    }
                    sceAudioSetChannelDataLen(index, PSP_AUDIO_SAMPLE_ALIGN(samplecount));
                    channel->SetPosition(0);

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
            sceKernelDelayThread(10000);
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
PSPAudioRenderer::CalculateVolumesFromPanning(PanningMode mode, float volume, float panning, int& leftVolume, int& rightVolume)
{
    switch (mode)
    {
        case PAN_CLAMPEDLINEAR:
            {
                float leftGain = 1.0f - panning;
                float rightGain = panning + 1.0f;
                leftVolume = (int)((float)PSP_AUDIO_VOLUME_MAX * volume * ((leftGain > 1.0f) ? 1.0f : leftGain));
                rightVolume = (int)((float)PSP_AUDIO_VOLUME_MAX * volume * ((rightGain > 1.0f) ? 1.0f : rightGain));
            }
            break;

        case PAN_CONSTANTPOWER:
            {
                float var = 0.785398f * (panning + 1.0f);
                leftVolume = (int)((float)PSP_AUDIO_VOLUME_MAX * volume * core::Math::Cos(var));
                rightVolume = (int)((float)PSP_AUDIO_VOLUME_MAX * volume * core::Math::Sin(var));
            }
            break;

        default:
            CE_ASSERT(false, "PSPAudioRenderer::CalculateVolumesFromPanning(): panningmode '%i' not supported\n", mode);
    }
}

} // namespace chrissly