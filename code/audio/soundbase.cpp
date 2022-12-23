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
    inUse(false),
    requestRelease(false),
    useCount(0U)
{

}

//------------------------------------------------------------------------------
/**
*/
SoundBase::~SoundBase()
{
    this->ReleaseInternal();
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
        this->ReleaseInternal();
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
SoundBase::Setup(const char* const filename, Mode modeflags, Codec* const codec)
{
    this->mode = modeflags;
    this->audioCodec = codec;
    this->audioCodec->SetupSound(filename, modeflags, &this->sampleBuffer, this->lengthInSamples, this->audioFormat, this->soundType, this->numChannels, this->bitsPerSample, this->sampleRate);
    this->CreateInternalResources();
    this->inUse = true;
}

//------------------------------------------------------------------------------
/**
*/
void
SoundBase::CreateInternalResources()
{

}

//------------------------------------------------------------------------------
/**
*/
bool
SoundBase::IsInUse() const
{
    return this->inUse;
}

//------------------------------------------------------------------------------
/**
*/
void
SoundBase::IncrementUseCount()
{
    this->releaseSyncLock.Lock();

    ++this->useCount;

    this->releaseSyncLock.Unlock();
}

//------------------------------------------------------------------------------
/**
*/
void
SoundBase::DecrementUseCount()
{
    this->releaseSyncLock.Lock();

    if (this->useCount > 0U)
    {
        --this->useCount;
    }

    if (this->requestRelease && 0U == this->useCount)
    {
        this->ReleaseInternal();
    }

    this->releaseSyncLock.Unlock();
}

//------------------------------------------------------------------------------
/**
*/
void*
SoundBase::GetSampleBufferPointer(unsigned int position) const
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
                return (void*)((uintptr_t)this->sampleBuffer + (uintptr_t)position * (uintptr_t)this->numChannels);
            case AUDIO_FORMAT_PCM16:
                return (void*)((uintptr_t)this->sampleBuffer + ((uintptr_t)position << 1U) * (uintptr_t)this->numChannels);
            default:
                CE_ASSERT(false, "SoundBase::GetSampleBufferPointer(): audio format not supported\n");
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------
/**
*/
Codec* const
SoundBase::GetCodec() const
{
    return this->audioCodec;
}

//------------------------------------------------------------------------------
/**
*/
void
SoundBase::ReleaseInternal()
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

    this->inUse = false;

    this->requestRelease = false;
}

} // namespace audio
} // namespace chrissly