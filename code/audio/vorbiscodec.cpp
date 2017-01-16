//------------------------------------------------------------------------------
//  vorbiscodec.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "vorbiscodec.h"
#include "fswrapper.h"
#include "memoryallocatorconfig.h"
#include "debug.h"
#include "stb_vorbis.c"

namespace chrissly
{
namespace audio
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
VorbisCodec::VorbisCodec()
{

}

//------------------------------------------------------------------------------
/**
*/
VorbisCodec::~VorbisCodec()
{

}

//------------------------------------------------------------------------------
/**
*/
void
VorbisCodec::SetupSound(const char* filename, Mode mode, void** sampleBuffer, unsigned int& length, AudioFormat& format, SoundType& type, int& channels, int& bits, unsigned int& sampleRate)
{
    FileHandle fd = FSWrapper::Open(filename, ReadAccess, Buffer, 0777);

    if (mode & MODE_CREATESTREAM || mode & MODE_CREATECOMPRESSEDSAMPLE)
    {
        FSWrapper::Close(fd);
        CE_ASSERT(false, "VorbisCodec::SetupSound(): vorbis codec does not support streaming\n");
    }
    else
    {
        unsigned int fileSize = FSWrapper::GetFileSize(fd);
        void* fileBuffer = CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, fileSize);
        CE_ASSERT(fileBuffer != NULL, "VorbisCodec::SetupSound(): failed to allocate '%i' bytes for samplebuffer\n", fileSize);
        FSWrapper::Read(fd, fileBuffer, fileSize);
        FSWrapper::Close(fd);
        int samplingRate;
        length = stb_vorbis_decode_memory((unsigned char*)fileBuffer, (int)fileSize, &channels, &samplingRate, (short**)sampleBuffer);
        sampleRate = (unsigned int)samplingRate;
        CE_FREE(fileBuffer);
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
void*
VorbisCodec::FillStreamBuffer(unsigned int numSamples, unsigned int position)
{
    CE_UNREFERENCED_PARAMETER(numSamples);
    CE_UNREFERENCED_PARAMETER(position);

    return NULL;
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
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
VorbisCodec::EndOfStream() const
{
    return false;
}

} // namespace audio
} // namespace chrissly