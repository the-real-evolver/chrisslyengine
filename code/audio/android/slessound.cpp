//------------------------------------------------------------------------------
//  slessound.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "slessound.h"
#include "debug.h"

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
SLESSound::SLESSound()
{

}

//------------------------------------------------------------------------------
/**
*/
SLESSound::~SLESSound()
{

}

//------------------------------------------------------------------------------
/**
*/
void
SLESSound::_CreateInternalResources()
{
    // setup the format of the content in the buffer queue
    this->pcm.formatType    = SL_DATAFORMAT_PCM;
    this->pcm.containerSize = 16;
    this->pcm.endianness    = SL_BYTEORDER_LITTLEENDIAN;
    this->pcm.numChannels   = this->numChannels;
    switch (this->sampleRate)
    {
        case 8000:
            this->pcm.samplesPerSec = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            this->pcm.samplesPerSec = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            this->pcm.samplesPerSec = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            this->pcm.samplesPerSec = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            this->pcm.samplesPerSec = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            this->pcm.samplesPerSec = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            this->pcm.samplesPerSec = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            this->pcm.samplesPerSec = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            this->pcm.samplesPerSec = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            this->pcm.samplesPerSec = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            this->pcm.samplesPerSec = SL_SAMPLINGRATE_88_2;
            break;
        case 192000:
            this->pcm.samplesPerSec = SL_SAMPLINGRATE_192;
            break;
        default:
            CE_ASSERT(false, "SLESSound::_CreateInternalResources(): %u kHz samplingrate not supported\n", this->sampleRate);
    }
    switch (this->audioFormat)
    {
        case audio::AUDIO_FORMAT_PCM8:
            this->pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_8;
            break;
        case audio::AUDIO_FORMAT_PCM16:
            this->pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
            break;
        default:
            CE_ASSERT(false, "SLESSound::_CreateInternalResources(): audio format not supported\n");
    }
    switch (this->numChannels)
    {
        case 1:
            this->pcm.channelMask = SL_SPEAKER_FRONT_CENTER;
            break;
        case 2:
            this->pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
            break;
        default:
            CE_ASSERT(false, "SLESSound::_CreateInternalResources(): %i channel audio output not supported\n", this->numChannels);
    }

    // setup the data source structure for the buffer queue
    this->bufferQueue.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
    this->bufferQueue.numBuffers  = 2;

    this->audioSource.pFormat  = &this->pcm;
    this->audioSource.pLocator = &this->bufferQueue;
}

//------------------------------------------------------------------------------
/**
*/
SLDataSource
SLESSound::GetAudioSource() const
{
    return this->audioSource;
}

} // namespace chrissly
