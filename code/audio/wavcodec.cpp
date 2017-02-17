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

static const unsigned int MaxStreamBufferSamples = 2048U;

//------------------------------------------------------------------------------
/**
*/
WavCodec::WavCodec() :
    riffWavHeaderSize(0),
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
    this->streamBuffers[0] = NULL;
    this->streamBuffers[1] = NULL;
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
WavCodec::SetupSound(const char* filename, Mode mode, void** sampleBuffer, unsigned int& length, AudioFormat& format, SoundType& type, int& channels, int& bits, unsigned int& sampleRate)
{
    FileHandle fd = FSWrapper::Open(filename, ReadAccess, (mode & MODE_CREATESTREAM) ? Streaming : Random, 0777);

    char chunkID[4] = {'\0'};
    unsigned int chunkSize, byteRate;

    // "RIFF" chunk descriptor
    FSWrapper::Read(fd, chunkID, 4);
    CE_ASSERT(0 == strncmp(chunkID, "RIFF", 4), "WavCodec::SetupSound(): '%s' not a RIFF file\n", filename);
    FSWrapper::Read(fd, &chunkSize, 4);
    FSWrapper::Read(fd, chunkID, 4);
    CE_ASSERT(0 == strncmp(chunkID, "WAVE", 4), "WavCodec::SetupSound(): '%s' RIFF file but not a wave file\n", filename);
    this->riffWavHeaderSize = 12;

    short audioFormat, numChannels = 0, blockAlign, bitsPerSample = 0;

    while (FSWrapper::Read(fd, chunkID, 4) > 0)
    {
        FSWrapper::Read(fd, &chunkSize, 4);
        this->riffWavHeaderSize += 8;
        if (0 == strncmp(chunkID, "fmt ", 4))
        {
            // "fmt " sub-chunk
            FSWrapper::Read(fd, &audioFormat, 2);   // WAVE_FORMAT_PCM = 0x0001, WAVE_FORMAT_IEEE_FLOAT = 0x0003, WAVE_FORMAT_ALAW = 0x0006, WAVE_FORMAT_MULAW = 0x0007, WAVE_FORMAT_EXTENSIBLE = 0xFFFF
            FSWrapper::Read(fd, &numChannels, 2);   // Mono = 1, Stereo = 2, etc.
            FSWrapper::Read(fd, &sampleRate, 4);    // 44100, 22050, etc.
            FSWrapper::Read(fd, &byteRate, 4);      // sampleRate * numChannels * bitsPerSample / 8
            FSWrapper::Read(fd, &blockAlign, 2);    // numChannels * bitsPerSample / 8
            FSWrapper::Read(fd, &bitsPerSample, 2); // 8 bits = 8, 16 bits = 16, etc.
            if (chunkSize > 16)
            {
                FSWrapper::Seek(fd, chunkSize - 16, Current);
            }
            this->riffWavHeaderSize += chunkSize;
        }
        else if (0 == strncmp(chunkID, "data", 4))
        {
            // "data" sub-chunk
            if (8 == bitsPerSample)
            {
                length = chunkSize;
                format = AUDIO_FORMAT_PCM8;
                this->bytesPerSample = (unsigned char)numChannels;
            }
            else if (16 == bitsPerSample)
            {
                length = chunkSize >> 1U;
                format = AUDIO_FORMAT_PCM16;
                this->bytesPerSample = 2 * (unsigned char)numChannels;
            }
            else
            {
                CE_ASSERT(false, "WavCodec::SetupSound(): '%i' bit samples not supported\n", bitsPerSample);
            }
            length = length / numChannels;
            type = SOUND_TYPE_WAV;
            channels = numChannels;
            bits = bitsPerSample;

            if (mode & MODE_CREATESTREAM)
            {
                this->openedAsStream = true;
                this->streamFileHandle = fd;
                this->lengthInBytes = chunkSize;
                this->streamBuffers[0] = CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, MaxStreamBufferSamples * this->bytesPerSample);
                this->streamBuffers[1] = CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, MaxStreamBufferSamples * this->bytesPerSample);
                *sampleBuffer = NULL;
            }
            else
            {
                void* buffer = CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, chunkSize);
                CE_ASSERT(buffer != NULL, "WavCodec::SetupSound(): failed to allocate '%i' bytes for samplebuffer\n", chunkSize);
                FSWrapper::Read(fd, buffer, chunkSize); // read the actual sound data
                FSWrapper::Close(fd);
                *sampleBuffer = buffer;
            }

            break;
        }
        else
        {
            // skip information chunks like "fact" etc.
            FSWrapper::Seek(fd, chunkSize, Current);
            this->riffWavHeaderSize += chunkSize;
        }
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
    FSWrapper::Seek(this->streamFileHandle, this->riffWavHeaderSize, Begin);
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
    if (this->endOfStream)
    {
        return;
    }

    if (!this->backBufferFilled)
    {
        unsigned int bytesToLoadThisFrame = this->bytesToLoadToBackBuffer;
        if ((this->seekPosition + this->bytesToLoadToBackBuffer) > this->lengthInBytes)
        {
            bytesToLoadThisFrame = this->lengthInBytes - this->seekPosition;
        }

        FSWrapper::Read(this->streamFileHandle, this->streamBuffers[this->currentStreamBufferIndex ^ 1], bytesToLoadThisFrame);
        this->seekPosition += bytesToLoadThisFrame;
        this->bytesToLoadToBackBuffer = bytesToLoadThisFrame;
        this->backBufferFilled = true;
    }
}

//------------------------------------------------------------------------------
/**
*/
void*
WavCodec::FillStreamBuffer(unsigned int numSamples, unsigned int position)
{
    CE_UNREFERENCED_PARAMETER(numSamples);
    CE_UNREFERENCED_PARAMETER(position);

    return NULL;
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
    if (this->seekPosition >= this->lengthInBytes)
    {
        this->endOfStream = true;
    }
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