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
    this->pcm.samplesPerSec = SL_SAMPLINGRATE_44_1;
    this->pcm.containerSize = 16;
    this->pcm.endianness    = SL_BYTEORDER_LITTLEENDIAN;
    this->pcm.numChannels   = this->numChannels;
    switch (this->format)
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
