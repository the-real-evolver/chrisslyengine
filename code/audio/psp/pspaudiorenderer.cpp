//------------------------------------------------------------------------------
//  pspaudiorenderer.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "pspaudiorenderer.h"
#include "modeflags.h"
#include <stdio.h>
#include <pspaudio.h>

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
PSPAudioRenderer::StartChannel(audio::Channel* channel)
{
    int index;
    channel->GetIndex(&index);
    audio::Sound* sound;
    channel->GetCurrentSound(&sound);
    unsigned int length;
    sound->GetLength(&length);
    int numChannels;
    sound->GetFormat(NULL, &numChannels, NULL);

    int samplecount = (length >= PSP_AUDIO_SAMPLE_MAX) ? PSP_AUDIO_SAMPLE_MAX : length;
    samplecount = PSP_AUDIO_SAMPLE_ALIGN(samplecount);
    PspAudioFormats audioFormat;
    switch (numChannels)
    {
        case 1: audioFormat = PSP_AUDIO_FORMAT_MONO; break;
        case 2: audioFormat = PSP_AUDIO_FORMAT_STEREO; break;
        default: audioFormat = PSP_AUDIO_FORMAT_MONO; break;
    }
    index = sceAudioChReserve(index, samplecount, audioFormat);

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
                samplecount = PSP_AUDIO_SAMPLE_ALIGN(samplesRemaining);
                sceAudioChRelease(index);
                int numChannels;
                sound->GetFormat(NULL, &numChannels, NULL);
                PspAudioFormats audioFormat;
                switch (numChannels)
                {
                    case 1: audioFormat = PSP_AUDIO_FORMAT_MONO; break;
                    case 2: audioFormat = PSP_AUDIO_FORMAT_STEREO; break;
                    default: audioFormat = PSP_AUDIO_FORMAT_MONO; break;
                }
                index = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL, samplecount, audioFormat);
                channel->_SetIndex(index);
            }

            if (index >= 0)
            {
                void* bufferPositon = sound->_GetStaticSampleBufferPointer(position);
                sceAudioOutput(index, PSP_AUDIO_VOLUME_MAX, bufferPositon);
                channel->_SetIsPlaying(true);
                channel->SetPosition(position += samplecount);
            }
        }
        else
        {
            sceAudioChRelease(index);
            audio::Mode mode;
            sound->GetMode(&mode);
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

} // namespace chrissly