//------------------------------------------------------------------------------
//  soundbase.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "soundbase.h"
#include "memoryallocatorconfig.h"
#include "debug.h"
#include <stdio.h>

namespace chrissly
{
namespace audio
{

//------------------------------------------------------------------------------
/**
*/
SoundBase::SoundBase() :
    mode(MODE_DEFAULT),
    soundType(SOUND_TYPE_UNKNOWN),
    audioFormat(AUDIO_FORMAT_NONE),
    lengthInSamples(0U),
    numChannels(0),
    bitsPerSample(0),
    sampleRate(0U),
    sampleBuffer(NULL),
    audioCodec(NULL),
    realized(false),
    requestRelease(false),
    useCount(0U)
{

}

//------------------------------------------------------------------------------
/**
*/
SoundBase::~SoundBase()
{
    this->_Release();
}

//------------------------------------------------------------------------------
/**
*/
Result
SoundBase::GetLength(unsigned int* const length)
{
    *length = this->lengthInSamples;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
SoundBase::GetFormat(SoundType* const type, AudioFormat* const format, int* const channels, int* const bits)
{
    if (type != NULL)
    {
        *type = this->soundType;
    }
    if (format != NULL)
    {
        *format = this->audioFormat;
    }
    if (channels != NULL)
    {
        *channels = this->numChannels;
    }
    if (bits != NULL)
    {
        *bits = this->bitsPerSample;
    }
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
SoundBase::GetMode(Mode* const modeflags)
{
    *modeflags = this->mode;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
SoundBase::Release()
{
    this->releaseSyncLock.Lock();

    if (0 == this->useCount)
    {
        this->_Release();
    }
    else
    {
        this->requestRelease = true;
    }

    this->releaseSyncLock.Unlock();

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
void
SoundBase::_Setup(const char* const filename, Mode modeflags, Codec* const codec)
{
    this->mode = modeflags;
    this->audioCodec = codec;
    this->audioCodec->SetupSound(filename, modeflags, &this->sampleBuffer, this->lengthInSamples, this->audioFormat, this->soundType, this->numChannels, this->bitsPerSample, this->sampleRate);
    this->_CreateInternalResources();
    this->realized = true;
}

//------------------------------------------------------------------------------
/**
*/
void
SoundBase::_Release()
{
    this->mode = MODE_DEFAULT;
    this->soundType = SOUND_TYPE_UNKNOWN;
    this->audioFormat = AUDIO_FORMAT_NONE;
    this->lengthInSamples = 0U;
    this->numChannels = 0;
    this->bitsPerSample = 0;
    this->sampleRate = 0U;

    if (this->sampleBuffer != NULL)
    {
        CE_FREE(this->sampleBuffer);
        this->sampleBuffer = NULL;
    }
    if (this->audioCodec != NULL)
    {
        CE_DELETE this->audioCodec;
        this->audioCodec = NULL;
    }

    this->realized = false;

    this->requestRelease = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
SoundBase::_IsRealized() const
{
    return this->realized;
}

//------------------------------------------------------------------------------
/**
*/
void
SoundBase::_IncrementUseCount()
{
    this->releaseSyncLock.Lock();

    ++this->useCount;

    this->releaseSyncLock.Unlock();
}

//------------------------------------------------------------------------------
/**
*/
void
SoundBase::_DecrementUseCount()
{
    this->releaseSyncLock.Lock();

    if (this->useCount > 0U)
    {
        --this->useCount;
    }

    if (this->requestRelease && 0U == this->useCount)
    {
        this->_Release();
    }

    this->releaseSyncLock.Unlock();
}

//------------------------------------------------------------------------------
/**
*/
void*
SoundBase::_GetSampleBufferPointer(unsigned int position) const
{
    if (this->mode & MODE_CREATESTREAM)
    {
        return NULL;
    }
    else if (this->mode & MODE_CREATECOMPRESSEDSAMPLE && this->soundType != SOUND_TYPE_WAV)
    {
        return NULL;
    }
    else if (MODE_DEFAULT == this->mode || this->mode & MODE_CREATESAMPLE || SOUND_TYPE_WAV == this->soundType)
    {
        switch (this->audioFormat)
        {
            case AUDIO_FORMAT_PCM8:
                return (void*)((uintptr_t)this->sampleBuffer + position * this->numChannels);
            case AUDIO_FORMAT_PCM16:
                return (void*)((uintptr_t)this->sampleBuffer + (position << 1U) * this->numChannels);
            default:
                CE_ASSERT(false, "SoundBase::_GetSampleBufferPointer(): audio format not supported\n");
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------
/**
*/
Codec* const
SoundBase::_GetCodec() const
{
    return this->audioCodec;
}

//------------------------------------------------------------------------------
/**
*/
void
SoundBase::_CreateInternalResources()
{

}

} // namespace audio
} // namespace chrissly