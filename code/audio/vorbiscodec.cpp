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
bool
VorbisCodec::SetupSound(const char* filename, Mode mode, void** sampleBuffer, unsigned int& length, AudioFormat& format, SoundType& type, int& channels, int& bits)
{
    if (MODE_DEFAULT == mode || mode & MODE_CREATESAMPLE || mode & MODE_CREATECOMPRESSEDSAMPLE)
    {
        FileHandle fd = FSWrapper::Open(filename, ReadAccess, 0777);
        unsigned int fileSize = FSWrapper::GetFileSize(fd);
        void* fileBuffer = CE_MALLOC_ALIGN(16, fileSize);
        FSWrapper::Read(fd, fileBuffer, fileSize);
        FSWrapper::Close(fd);

        length = stb_vorbis_decode_memory((unsigned char*)fileBuffer, fileSize, &channels, (short int**)sampleBuffer);
        CE_FREE(fileBuffer);

        type = SOUND_TYPE_OGGVORBIS;
        format = AUDIO_FORMAT_PCM16;
        bits = 16;

        return true;
    }

    return false;
}

} // namespace audio
} // namespace chrissly