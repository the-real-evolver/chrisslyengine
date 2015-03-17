//------------------------------------------------------------------------------
//  pspaudiorenderer.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "pspaudiorenderer.h"
#include "modeflags.h"
#include "sound.h"
#include "chrisslymath.h"
#include "debug.h"
#include <stdio.h>
#include <pspthreadman.h>

namespace chrissly
{

PSPAudioRenderer* PSPAudioRenderer::Singleton = NULL;

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
    int error = sceKernelWaitSema(channel->GetSemaphoreId(), 1, 0);
    CE_ASSERT(error >= 0, "PSPAudioRenderer::StartChannel(): sceKernelWaitSema() failed: %08x\n", error);

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
        samplecount = PSP_AUDIO_SAMPLE_MAX;
    }
    if (length < samplecount) samplecount = length;

    index = sceAudioChReserve(index, PSP_AUDIO_SAMPLE_ALIGN(samplecount), format);
    channel->_SetIndex(index);
    if (index != audio::Channel::CHANNEL_FREE) channel->_SetIsPlaying(true);

    error = sceKernelSignalSema(channel->GetSemaphoreId(), 1);
    CE_ASSERT(error >= 0, "PSPAudioRenderer::StartChannel(): sceKernelSignalSema() failed: %08x\n", error);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPAudioRenderer::UpdateChannel(audio::Channel* channel)
{
    bool isPlaying;
    channel->IsPlaying(&isPlaying);

    if (isPlaying && channel->_PropertiesHasChanged())
    {
        int index;
        channel->GetIndex(&index);
        float volume;
        channel->GetVolume(&volume);
        float panning;
        channel->GetPan(&panning);
        int leftVolume, rightVolume;
        CalculateVolumesFromPanning(PAN_CLAMPEDLINEAR, volume, panning, leftVolume, rightVolume);
        sceAudioChangeChannelVolume(index, leftVolume, rightVolume);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PSPAudioRenderer::ReleaseChannel(audio::Channel* channel)
{
    int error = sceKernelWaitSema(channel->GetSemaphoreId(), 1, 0);
    CE_ASSERT(error >= 0, "PSPAudioRenderer::ReleaseChannel(): sceKernelWaitSema() failed: %08x\n", error);

    channel->RequestStop();
    channel->_SetIsPlaying(false);

    error = sceKernelSignalSema(channel->GetSemaphoreId(), 1);
    CE_ASSERT(error >= 0, "PSPAudioRenderer::ReleaseChannel(): sceKernelSignalSema() failed: %08x\n", error);
}

//------------------------------------------------------------------------------
/**
*/
int
PSPAudioRenderer::ChannelThread(SceSize args, void* argp)
{
    audio::Channel* channel = (audio::Channel*)(*(unsigned int*)argp);
    while (true)
    {
        int error = sceKernelWaitSema(channel->GetSemaphoreId(), 1, 0);
        CE_ASSERT(error >= 0, "PSPAudioRenderer::ChannelThread(): sceKernelWaitSema() failed: %08x\n", error);

        int index;
        channel->GetIndex(&index);

        if (channel->GetStopRequest())
        {
            sceAudioChangeChannelVolume(index, 0, 0);
            sceAudioChRelease(index);
            channel->_SetIndex(audio::Channel::CHANNEL_FREE);
        }

        bool paused;
        channel->GetPaused(&paused);
        if (paused)
        {
            error = sceKernelSignalSema(channel->GetSemaphoreId(), 1);
            CE_ASSERT(error >= 0, "PSPAudioRenderer::ChannelThread(): sceKernelSignalSema() state[paused] failed: %08x\n", error);
            sceKernelDelayThread(10000);
            continue;
        }

        bool isPlaying;
        channel->IsPlaying(&isPlaying);
        if (isPlaying)
        {
            audio::Sound* sound;
            channel->GetCurrentSound(&sound);
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
                    samplecount = codec->GetStreamBufferLength() / (numChannels * (bits >> 3));
                }
                else
                {
                    samplecount = PSP_AUDIO_SAMPLE_MAX;
                }
                if (samplesRemaining < samplecount)
                {
                    samplecount = PSP_AUDIO_SAMPLE_ALIGN(samplesRemaining);
                    sceAudioSetChannelDataLen(index, samplecount);
                }
                channel->SetPosition(position + samplecount);

                float volume;
                channel->GetVolume(&volume);
                float panning;
                channel->GetPan(&panning);
                int leftVolume, rightVolume;
                CalculateVolumesFromPanning(PAN_CLAMPEDLINEAR, volume, panning, leftVolume, rightVolume);

                error = sceKernelSignalSema(channel->GetSemaphoreId(), 1);
                CE_ASSERT(error >= 0, "PSPAudioRenderer::ChannelThread(): sceKernelSignalSema() state[playing] failed: %08x\n", error);

                if (mode & audio::MODE_CREATESTREAM)
                {
                    if (position > 0) codec->SwapStreamBuffers();
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
                        samplecount = PSP_AUDIO_SAMPLE_MAX;
                    }
                    if (length < samplecount) samplecount = length;

                    channel->SetPosition(0);
                    sceAudioSetChannelDataLen(index, PSP_AUDIO_SAMPLE_ALIGN(samplecount));

                    error = sceKernelSignalSema(channel->GetSemaphoreId(), 1);
                    CE_ASSERT(error >= 0, "PSPAudioRenderer::ChannelThread(): sceKernelSignalSema() state[restart loop] failed: %08x\n", error);
                }
                else
                {
                    sceAudioChRelease(index);
                    channel->_SetIndex(audio::Channel::CHANNEL_FREE);
                    channel->_SetIsPlaying(false);

                    error = sceKernelSignalSema(channel->GetSemaphoreId(), 1);
                    CE_ASSERT(error >= 0, "PSPAudioRenderer::ChannelThread(): sceKernelSignalSema() state[stopping] failed: %08x\n", error);
                }
            }
        }
        else
        {
            error = sceKernelSignalSema(channel->GetSemaphoreId(), 1);
            CE_ASSERT(error >= 0, "PSPAudioRenderer::ChannelThread(): sceKernelSignalSema() state[idling] failed: %08x\n", error);
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
                float var = 3.141593f * (panning + 1.0f) / 4.0f;
                leftVolume = (int)((float)PSP_AUDIO_VOLUME_MAX * volume * core::Math::Cos(var));
                rightVolume = (int)((float)PSP_AUDIO_VOLUME_MAX * volume * core::Math::Sin(var));
            }
            break;

        default:
            CE_ASSERT(false, "PSPAudioRenderer::CalculateVolumesFromPanning(): panningmode '%i' not supported\n", mode);
    }
}

} // namespace chrissly