//------------------------------------------------------------------------------
//  sound.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "sound.h"
#include "memoryallocatorconfig.h"
#include <stdio.h>

namespace chrissly
{
namespace audio
{

//------------------------------------------------------------------------------
/**
*/
Sound::Sound() :
    mode(MODE_DEFAULT),
    format(AUDIO_FORMAT_NONE),
    length(0),
    numChannels(0),
    bitsPerSample(0),
    sampleBuffer(NULL),
    codec(NULL)
{

}

//------------------------------------------------------------------------------
/**
*/
Sound::~Sound()
{
    this->Release();
}

//------------------------------------------------------------------------------
/**
*/
bool
Sound::GetLength(unsigned int* length)
{
    *length = this->length;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
Sound::GetFormat(AudioFormat* format, int* channels, int* bits)
{
    if (format != NULL) *format = this->format;
    if (channels != NULL) *channels = this->numChannels;
    if (bits != NULL) *bits = this->bitsPerSample;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
Sound::GetMode(Mode* mode)
{
    *mode = this->mode;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
Sound::Release()
{
    this->mode = MODE_DEFAULT;
    this->format = AUDIO_FORMAT_NONE;
    this->length = 0;
    this->numChannels = 0;
    this->bitsPerSample = 0;

    if (this->sampleBuffer != NULL)
    {
        CE_FREE(this->sampleBuffer);
        this->sampleBuffer = NULL;
    }
    if (this->codec != NULL)
    {
        CE_DELETE this->codec;
        this->codec = NULL;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
Sound::_Setup(const char* filename, Mode mode, Codec* codec)
{
    this->mode = mode;
    this->codec = codec;
    this->codec->SetupSound(filename, mode, &this->sampleBuffer, this->length, this->format, this->numChannels, this->bitsPerSample);
}

//------------------------------------------------------------------------------
/**
*/
void*
Sound::_GetStaticSampleBufferPointer(unsigned int position)
{
    switch (this->format)
    {
        case AUDIO_FORMAT_PCM16:
            return (void*)((unsigned int)this->sampleBuffer + (position << 1) * this->numChannels);
        default:
            return NULL;
    }
}

} // namespace audio
} // namespace chrissly