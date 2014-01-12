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
Result
Sound::GetLength(unsigned int* length)
{
    *length = this->length;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
Sound::GetFormat(SoundType* type, AudioFormat* format, int* channels, int* bits)
{
    if (type != NULL) *type = this->type;
    if (format != NULL) *format = this->format;
    if (channels != NULL) *channels = this->numChannels;
    if (bits != NULL) *bits = this->bitsPerSample;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
Sound::GetMode(Mode* mode)
{
    *mode = this->mode;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
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

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
void
Sound::_Setup(const char* filename, Mode mode, Codec* codec)
{
    this->mode = mode;
    this->codec = codec;
    this->codec->SetupSound(filename, mode, &this->sampleBuffer, this->length, this->format, this->type, this->numChannels, this->bitsPerSample);
}

//------------------------------------------------------------------------------
/**
*/
void*
Sound::_GetSampleBufferPointer(unsigned int position)
{
    if (this->mode & MODE_CREATESTREAM)
    {
        return NULL;
    }
    else if (this->mode & MODE_CREATECOMPRESSEDSAMPLE && this->type != SOUND_TYPE_WAV)
    {
        return NULL;
    }
    else if (MODE_DEFAULT == this->mode || this->mode & MODE_CREATESAMPLE || SOUND_TYPE_WAV == this->type)
    {
        switch (this->format)
        {
            case AUDIO_FORMAT_PCM16:
                return (void*)((unsigned int)this->sampleBuffer + (position << 1) * this->numChannels);
            default:
                return NULL;
        }
    }

    return NULL;
}

} // namespace audio
} // namespace chrissly