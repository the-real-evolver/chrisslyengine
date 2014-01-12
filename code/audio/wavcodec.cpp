//------------------------------------------------------------------------------
//  wavcodec.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "wavcodec.h"
#include "fswrapper.h"
#include "memoryallocatorconfig.h"
#include "debug.h"
#include <string.h>

namespace chrissly
{
namespace audio
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
WavCodec::WavCodec()
{

}

//------------------------------------------------------------------------------
/**
*/
WavCodec::~WavCodec()
{

}

//------------------------------------------------------------------------------
/**
*/
bool
WavCodec::SetupSound(const char* filename, Mode mode, void** sampleBuffer, unsigned int& length, AudioFormat& format, SoundType& type, int& channels, int& bits)
{
    if (MODE_DEFAULT == mode || mode & MODE_CREATESAMPLE || mode & MODE_CREATECOMPRESSEDSAMPLE)
    {
        FileHandle fd = FSWrapper::Open(filename, ReadAccess, 0777);
        unsigned int fileSize = FSWrapper::GetFileSize(fd);

        char chunkID[4];
        unsigned int chunkSize;

        // "RIFF" chunk descriptor
        FSWrapper::Read(fd, chunkID, 4);
        CE_ASSERT(0 == strncmp(chunkID, "RIFF", 4), "WavCodec::SetupSound(): '%s' not a RIFF file", filename);
        FSWrapper::Read(fd, &chunkSize, 4);
        FSWrapper::Read(fd, chunkID, 4);
        CE_ASSERT(0 == strncmp(chunkID, "WAVE", 4), "WavCodec::SetupSound(): '%s' RIFF file but not a wave file", filename);

        // "fmt " sub-chunk
        short audioFormat, numChannels, blockAlign, bitsPerSample;
        unsigned int sampleRate, byteRate, dataSize;
        FSWrapper::Read(fd, chunkID, 4);        // Contains the letters "fmt "
        FSWrapper::Read(fd, &chunkSize, 4);     // 16 for PCM. This is the size of the rest of the Subchunk which follows this number
        FSWrapper::Read(fd, &audioFormat, 2);   // PCM = 1 (i.e. Linear quantization) Values other than 1 indicate some form of compression
        FSWrapper::Read(fd, &numChannels, 2);   // Mono = 1, Stereo = 2, etc.
        FSWrapper::Read(fd, &sampleRate, 4);    // 44100, 22050, etc.
        FSWrapper::Read(fd, &byteRate, 4);      // sampleRate * numChannels * bitsPerSample / 8
        FSWrapper::Read(fd, &blockAlign, 2);    // numChannels * bitsPerSample / 8
        FSWrapper::Read(fd, &bitsPerSample, 2); // 8 bits = 8, 16 bits = 16, etc.

        // "data" sub-chunk
        FSWrapper::Read(fd, chunkID, 4);        // Contains the letters "data"
        FSWrapper::Read(fd, &chunkSize, 4);     // numSamples * numChannels * bitsPerSample / 8
        dataSize = fileSize - 44;
        void* buffer = CE_MALLOC_ALIGN(16, dataSize);
        CE_ASSERT(buffer != NULL, "WavCodec::SetupSound(): failed to allocate '%i' bytes for samplebuffer", dataSize);
        FSWrapper::Read(fd, buffer, dataSize);  // read the actual sound data

        FSWrapper::Close(fd);

        *sampleBuffer = buffer;
        if (8 == bitsPerSample)
        {
            length = dataSize;
            format = AUDIO_FORMAT_PCM8;
        }
        else if (16 == bitsPerSample)
        {
            length = dataSize >> 1;
            format = AUDIO_FORMAT_PCM16;
        }
        length = length / numChannels;
        type = SOUND_TYPE_WAV;
        channels = numChannels;
        bits = bitsPerSample;

        return true;
    }

    return false;
}

} // namespace audio
} // namespace chrissly