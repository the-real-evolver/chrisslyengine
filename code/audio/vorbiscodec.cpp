//------------------------------------------------------------------------------
//  vorbiscodec.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "vorbiscodec.h"
#include "fswrapper.h"
#include "memoryallocatorconfig.h"
#include "debug.h"
#include <string.h>

namespace chrissly
{
namespace audio
{

using namespace chrissly::core;

static const unsigned int MaxStreamBufferSamples = 2048U;

//------------------------------------------------------------------------------
/**
*/
VorbisCodec::VorbisCodec() :
    lengthInSamples(0U),
    openedAsStream(false),
    seekPosition(0U),
    currentStreamBufferIndex(0U),
    fileBuffer(NULL),
    vorbisStream(NULL)
{
    this->streamBuffers[0U] = NULL;
    this->streamBuffers[1U] = NULL;
    memset(&this->vorbisInfo, 0, sizeof(this->vorbisInfo));
}

//------------------------------------------------------------------------------
/**
*/
VorbisCodec::~VorbisCodec()
{
    if (this->openedAsStream)
    {
        stb_vorbis_close(this->vorbisStream);
        CE_FREE(this->fileBuffer);
        CE_FREE(this->streamBuffers[0U]);
        CE_FREE(this->streamBuffers[1U]);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
VorbisCodec::SetupSound(const char* const filename, Mode mode, void** sampleBuffer, unsigned int& length, AudioFormat& format, SoundType& type, int& channels, int& bits, unsigned int& sampleRate)
{
    FileHandle fd = FSWrapper::Open(filename, READ_ACCESS, BUFFER, 0777);
    unsigned int fileSize = FSWrapper::GetFileSize(fd);
    this->fileBuffer = CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, fileSize);
    CE_ASSERT(this->fileBuffer != NULL, "VorbisCodec::SetupSound(): failed to allocate '%i' bytes for file buffer '%s'\n", fileSize, filename);
    FSWrapper::Read(fd, this->fileBuffer, fileSize);
    FSWrapper::Close(fd);

    if (mode & MODE_CREATESTREAM || mode & MODE_CREATECOMPRESSEDSAMPLE)
    {
        this->vorbisStream = stb_vorbis_open_memory((unsigned char*)this->fileBuffer, (int)fileSize, NULL, NULL);
        CE_ASSERT(this->vorbisStream != NULL, "VorbisCodec::SetupSound(): failed to open vorbis '%s' from memory\n", filename);
        this->vorbisInfo = stb_vorbis_get_info(this->vorbisStream);
        this->lengthInSamples = stb_vorbis_stream_length_in_samples(this->vorbisStream);
        this->openedAsStream = true;
        this->streamBuffers[0U] = CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, MaxStreamBufferSamples * this->vorbisInfo.channels * 2U);
        this->streamBuffers[1U] = CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, MaxStreamBufferSamples * this->vorbisInfo.channels * 2U);
        length = this->lengthInSamples;
        channels = this->vorbisInfo.channels;
        sampleRate = this->vorbisInfo.sample_rate;
        *sampleBuffer = NULL;
    }
    else
    {
        int samplingRate;
        length = stb_vorbis_decode_memory((unsigned char*)this->fileBuffer, (int)fileSize, &channels, &samplingRate, (short**)sampleBuffer);
        sampleRate = (unsigned int)samplingRate;
        CE_FREE(this->fileBuffer);
    }

    format = AUDIO_FORMAT_PCM16;
    type = SOUND_TYPE_OGGVORBIS;
    bits = 16;
}

//------------------------------------------------------------------------------
/**
*/
void
VorbisCodec::InitialiseStream()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VorbisCodec::FillStreamBackBuffer()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VorbisCodec::SwapStreamBuffers()
{

}

//------------------------------------------------------------------------------
/**
*/
void*
VorbisCodec::GetStreamBufferPointer() const
{
    return NULL;
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
VorbisCodec::GetStreamBufferLength() const
{
    return 0U;
}

//------------------------------------------------------------------------------
/**
*/
bool
VorbisCodec::EndOfStream() const
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void*
VorbisCodec::FillStreamBuffer(unsigned int numSamples, unsigned int position)
{
    if (position < this->lengthInSamples)
    {
        if (this->seekPosition != position)
        {
            stb_vorbis_seek(this->vorbisStream, position);
            this->seekPosition = position;
        }
        unsigned int samplesToLoad = numSamples;
        if ((this->seekPosition + samplesToLoad) > this->lengthInSamples)
        {
            samplesToLoad = this->lengthInSamples - this->seekPosition;
        }
        this->currentStreamBufferIndex ^= 1U;
        stb_vorbis_get_samples_short_interleaved(this->vorbisStream, this->vorbisInfo.channels, (short*)this->streamBuffers[this->currentStreamBufferIndex], this->vorbisInfo.channels * (int)samplesToLoad);
        this->seekPosition += samplesToLoad;
    }
    return this->streamBuffers[this->currentStreamBufferIndex];
}

} // namespace audio
} // namespace chrissly