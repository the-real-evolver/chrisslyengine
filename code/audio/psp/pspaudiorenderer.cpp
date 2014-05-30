//------------------------------------------------------------------------------
//  pspaudiorenderer.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "pspaudiorenderer.h"
#include "modeflags.h"
#include "sound.h"
#include "debug.h"
#include <stdio.h>

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
    index = sceAudioChReserve(index, samplecount, this->GetFormat(numChannels));

    channel->_SetIndex(index);
    channel->_SetIsPlaying(false);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPAudioRenderer::UpdateChannel(audio::Channel* channel)
{
    bool paused;
    channel->GetPaused(&paused);
    if (paused) return;

    bool outputting;
    channel->IsPlaying(&outputting);
    int index;
    channel->GetIndex(&index);

    if (outputting)
    {
        if (sceAudioGetChannelRestLen(index) <= 0)
        {
            outputting = false;
        }
        else if (channel->_PropertiesHasChanged())
        {
            float volume;
            channel->GetVolume(&volume);
            float panning;
            channel->GetPan(&panning);
            int leftVolume, rightVolume;
            this->CalculateVolumesFromPanning(PAN_CLAMPEDLINEAR, volume, panning, leftVolume, rightVolume);
            sceAudioChangeChannelVolume(index, leftVolume, rightVolume);
        }
    }

    if (!outputting)
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
                sceAudioChRelease(index);
                int numChannels;
                sound->GetFormat(NULL, NULL, &numChannels, NULL);
                samplecount = PSP_AUDIO_SAMPLE_ALIGN(samplesRemaining);
                index = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL, samplecount, this->GetFormat(numChannels));
                channel->_SetIndex(index);
            }

            if (index >= 0)
            {
                float volume;
                channel->GetVolume(&volume);
                float panning;
                channel->GetPan(&panning);
                int leftVolume, rightVolume;
                this->CalculateVolumesFromPanning(PAN_CLAMPEDLINEAR, volume, panning, leftVolume, rightVolume);
                void* bufferPositon = sound->_GetSampleBufferPointer(position);
                sceAudioOutputPanned(index, leftVolume, rightVolume, bufferPositon);
                channel->_SetIsPlaying(true);
                channel->SetPosition(position += samplecount);
            }
            else
            {
                channel->_SetIsPlaying(false);
            }
        }
        else
        {
            sceAudioChRelease(index);
            audio::Mode mode;
            channel->GetMode(&mode);
            if (mode & audio::MODE_LOOP_NORMAL)
            {
                channel->SetPosition(0);
                this->StartChannel(channel);
            }
            else
            {
                channel->_SetIndex(audio::Channel::CHANNEL_FREE);
                channel->_SetIsPlaying(false);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PSPAudioRenderer::ReleaseChannel(audio::Channel* channel)
{
    int index;
    channel->GetIndex(&index);
    sceAudioChRelease(index);
    channel->_SetIsPlaying(false);
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
PSPAudioRenderer::GetFormat(int channels) const
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