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

static const unsigned int RiffWavHeaderSize = 44;
static const unsigned int MaxStreamBufferSamples = 2048;

//------------------------------------------------------------------------------
/**
*/
WavCodec::WavCodec() :
    lengthInBytes(0),
    bytesPerSample(0),
    openedAsStream(false),
    seekPosition(0),
    endOfStream(false),
    currentStreamBufferIndex(0),
    currentStreamBufferLength(0),
    bytesToLoadToBackBuffer(0),
    backBufferFilled(false)
{

}

//------------------------------------------------------------------------------
/**
*/
WavCodec::~WavCodec()
{
    if (this->openedAsStream)
    {
        FSWrapper::Close(this->streamFileHandle);
        CE_FREE(this->streamBuffers[0]);
        CE_FREE(this->streamBuffers[1]);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
WavCodec::SetupSound(const char* filename, Mode mode, void** sampleBuffer, unsigned int& length, AudioFormat& format, SoundType& type, int& channels, int& bits)
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
    dataSize = fileSize - RiffWavHeaderSize;

    if (8 == bitsPerSample)
    {
        length = dataSize;
        format = AUDIO_FORMAT_PCM8;
        this->bytesPerSample = numChannels;
    }
    else if (16 == bitsPerSample)
    {
        length = dataSize >> 1;
        format = AUDIO_FORMAT_PCM16;
        this->bytesPerSample = 2 * numChannels;
    }
    length = length / numChannels;
    type = SOUND_TYPE_WAV;
    channels = numChannels;
    bits = bitsPerSample;

    if (mode & MODE_CREATESTREAM)
    {
        this->openedAsStream = true;
        this->streamFileHandle = fd;
        this->lengthInBytes = dataSize;
        this->streamBuffers[0] = CE_MALLOC_ALIGN(16, MaxStreamBufferSamples * this->bytesPerSample);
        this->streamBuffers[1] = CE_MALLOC_ALIGN(16, MaxStreamBufferSamples * this->bytesPerSample);
        *sampleBuffer = NULL;
    }
    else
    {
        void* buffer = CE_MALLOC_ALIGN(16, dataSize);
        CE_ASSERT(buffer != NULL, "WavCodec::SetupSound(): failed to allocate '%i' bytes for samplebuffer", dataSize);
        FSWrapper::Read(fd, buffer, dataSize); // read the actual sound data
        FSWrapper::Close(fd);
        *sampleBuffer = buffer;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
WavCodec::InitialiseStream()
{
    this->endOfStream = false;
    this->currentStreamBufferIndex = 0;
    this->bytesToLoadToBackBuffer = this->lengthInBytes < MaxStreamBufferSamples * this->bytesPerSample ? this->lengthInBytes : MaxStreamBufferSamples * this->bytesPerSample;
    this->seekPosition = this->bytesToLoadToBackBuffer;
    FSWrapper::Seek(this->streamFileHandle, RiffWavHeaderSize, Begin);
    FSWrapper::Read(this->streamFileHandle, this->streamBuffers[0], this->bytesToLoadToBackBuffer);
    this->currentStreamBufferLength = this->bytesToLoadToBackBuffer;
    this->backBufferFilled = false;
}

//------------------------------------------------------------------------------
/**
*/
void
WavCodec::FillStreamBackBuffer()
{
    if (this->endOfStream) return;

    unsigned int bytesToLoadThisFrame = this->bytesToLoadToBackBuffer;
    if ((this->seekPosition + this->bytesToLoadToBackBuffer) > this->lengthInBytes)
    {
        bytesToLoadThisFrame = this->lengthInBytes - this->seekPosition;
    }

    if (!this->backBufferFilled)
    {
        FSWrapper::Read(this->streamFileHandle, this->streamBuffers[this->currentStreamBufferIndex ^ 1], bytesToLoadThisFrame);
        this->seekPosition += bytesToLoadThisFrame;
        this->bytesToLoadToBackBuffer = bytesToLoadThisFrame;
        this->backBufferFilled = true;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
WavCodec::SwapStreamBuffers()
{
    this->currentStreamBufferLength = this->bytesToLoadToBackBuffer;
    this->currentStreamBufferIndex ^= 1;
    this->backBufferFilled = false;
    if (this->seekPosition >= this->lengthInBytes) this->endOfStream = true;
}

//------------------------------------------------------------------------------
/**
*/
void*
WavCodec::GetStreamBufferPointer() const
{
    return this->streamBuffers[this->currentStreamBufferIndex];
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
WavCodec::GetStreamBufferLength() const
{
    return this->currentStreamBufferLength;
}

//------------------------------------------------------------------------------
/**
*/
bool
WavCodec::EndOfStream() const
{
    return this->endOfStream;
}

} // namespace audio
} // namespace chrissly