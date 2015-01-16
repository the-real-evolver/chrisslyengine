//------------------------------------------------------------------------------
//  pspaudiorenderer.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "pspaudiorenderer.h"
#include "modeflags.h"
#include "sound.h"
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
    int numChannels;
    sound->GetFormat(NULL, NULL, &numChannels, NULL);

    int samplecount = (length >= PSP_AUDIO_SAMPLE_MAX) ? PSP_AUDIO_SAMPLE_MAX : length;
    samplecount = PSP_AUDIO_SAMPLE_ALIGN(samplecount);
    index = sceAudioChReserve(index, samplecount, GetFormat(numChannels));

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

    int index;
    channel->GetIndex(&index);
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

            int samplesRemaining = length - position;
            if (samplesRemaining > 0)
            {
                int samplecount = PSP_AUDIO_SAMPLE_MAX;
                if (samplesRemaining < PSP_AUDIO_SAMPLE_MAX)
                {
                    samplecount = PSP_AUDIO_SAMPLE_ALIGN(samplesRemaining);
                    sceAudioSetChannelDataLen(index, samplecount);
                }

                float volume;
                channel->GetVolume(&volume);
                float panning;
                channel->GetPan(&panning);
                int leftVolume, rightVolume;
                CalculateVolumesFromPanning(PAN_CLAMPEDLINEAR, volume, panning, leftVolume, rightVolume);
                channel->SetPosition(position + samplecount);

                error = sceKernelSignalSema(channel->GetSemaphoreId(), 1);
                CE_ASSERT(error >= 0, "PSPAudioRenderer::ChannelThread(): sceKernelSignalSema() state[playing] failed: %08x\n", error);

                sceAudioOutputPannedBlocking(index, leftVolume, rightVolume, sound->_GetSampleBufferPointer(position));
            }
            else
            {
                sceAudioChRelease(index);
                audio::Mode mode;
                channel->GetMode(&mode);
                if (mode & audio::MODE_LOOP_NORMAL)
                {
                    channel->SetPosition(0);

                    error = sceKernelSignalSema(channel->GetSemaphoreId(), 1);
                    CE_ASSERT(error >= 0, "PSPAudioRenderer::ChannelThread(): sceKernelSignalSema() state[restart loop] failed: %08x\n", error);

                    PSPAudioRenderer::Instance()->StartChannel(channel);
                }
                else
                {
                    channel->_SetIndex(audio::Channel::CHANNEL_FREE);
                    channel->_SetIsPlaying(false);

                    error = sceKernelSignalSema(channel->GetSemaphoreId(), 1);
                    CE_ASSERT(error >= 0, "PSPAudioRenderer::ChannelThread(): sceKernelSignalSema() state[stoping] failed: %08x\n", error);
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

        default:
            CE_ASSERT(false, "PSPAudioRenderer::CalculateVolumesFromPanning(): panningmode '%i' not supported\n", mode);
    }
}

//------------------------------------------------------------------------------
/**
*/
PspAudioFormats
PSPAudioRenderer::GetFormat(int channels)
{
    switch (channels)
    {
        case 1:
            return PSP_AUDIO_FORMAT_MONO;
        case 2:
            return PSP_AUDIO_FORMAT_STEREO;
        default:
            CE_ASSERT(false, "PSPAudioRenderer::GetFormat(): %i channel audio output not supported\n", channels);
    }

    return PSP_AUDIO_FORMAT_MONO;
}

} // namespace chrissly