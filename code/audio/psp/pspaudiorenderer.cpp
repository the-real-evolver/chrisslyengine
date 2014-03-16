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
    channel->SetPosition(0);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPAudioRenderer::UpdateChannel(audio::Channel* channel)
{
    bool isPlaying;
    channel->IsPlaying(&isPlaying);
    int index;
    channel->GetIndex(&index);

    if (isPlaying)
    {
        if (sceAudioGetChannelRestLen(index) <= 0)
        {
            channel->_SetIsPlaying(false);
            isPlaying = false;
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

    if (!isPlaying)
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
        }
        else
        {
            sceAudioChRelease(index);
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

        default: CE_ASSERT(false, "PSPAudioRenderer::CalculateVolumesFromPanning(): panningmode '%i' not supported\n", mode);
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
        case 1: return PSP_AUDIO_FORMAT_MONO;
        case 2: return PSP_AUDIO_FORMAT_STEREO;
        default: return PSP_AUDIO_FORMAT_MONO;
    }

    return PSP_AUDIO_FORMAT_MONO;
}

} // namespace chrissly