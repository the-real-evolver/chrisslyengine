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
    riffWavHeaderSize(0U),
    lengthInBytes(0U),
    bytesPerSample(0U),
    openedAsStream(false),
    seekPosition(0U),
    endOfStream(false),
    currentStreamBufferIndex(0U),
    currentStreamBufferLength(0U),
    bytesToLoadToBackBuffer(0U),
    backBufferFilled(false)
{
    this->streamBuffers[0U] = NULL;
    this->streamBuffers[1U] = NULL;
}

//------------------------------------------------------------------------------
/**
*/
WavCodec::~WavCodec()
{
    if (this->openedAsStream)
    {
        FSWrapper::Close(this->streamFileHandle);
        CE_FREE(this->streamBuffers[0U]);
        CE_FREE(this->streamBuffers[1U]);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
WavCodec::SetupSound(const char* const filename, Mode mode, void** sampleBuffer, unsigned int& length, AudioFormat& format, SoundType& type, int& channels, int& bits, unsigned int& sampleRate)
{
    FileHandle fd = FSWrapper::Open(filename, ReadAccess, (mode & MODE_CREATESTREAM) ? Streaming : Random, 0777);

    char chunkID[4U] = {'\0'};
    unsigned int chunkSize, byteRate;

    // "RIFF" chunk descriptor
    FSWrapper::Read(fd, chunkID, 4U);
    CE_ASSERT(0 == strncmp(chunkID, "RIFF", 4U), "WavCodec::SetupSound(): '%s' not a RIFF file\n", filename);
    FSWrapper::Read(fd, &chunkSize, 4U);
    FSWrapper::Read(fd, chunkID, 4U);
    CE_ASSERT(0 == strncmp(chunkID, "WAVE", 4U), "WavCodec::SetupSound(): '%s' RIFF file but not a wave file\n", filename);
    this->riffWavHeaderSize = 12U;

    short audioFormat, numChannels = 0, blockAlign, bitsPerSample = 0;

    while (FSWrapper::Read(fd, chunkID, 4U) > 0)
    {
        FSWrapper::Read(fd, &chunkSize, 4U);
        this->riffWavHeaderSize += 8U;
        if (0 == strncmp(chunkID, "fmt ", 4U))
        {
            // "fmt " sub-chunk
            FSWrapper::Read(fd, &audioFormat, 2U);   // WAVE_FORMAT_PCM = 0x0001, WAVE_FORMAT_IEEE_FLOAT = 0x0003, WAVE_FORMAT_ALAW = 0x0006, WAVE_FORMAT_MULAW = 0x0007, WAVE_FORMAT_EXTENSIBLE = 0xFFFF
            FSWrapper::Read(fd, &numChannels, 2U);   // Mono = 1, Stereo = 2, etc.
            FSWrapper::Read(fd, &sampleRate, 4U);    // 44100, 22050, etc.
            FSWrapper::Read(fd, &byteRate, 4U);      // sampleRate * numChannels * bitsPerSample / 8
            FSWrapper::Read(fd, &blockAlign, 2U);    // numChannels * bitsPerSample / 8
            FSWrapper::Read(fd, &bitsPerSample, 2U); // 8 bits = 8, 16 bits = 16, etc.
            if (chunkSize > 16U)
            {
                FSWrapper::Seek(fd, chunkSize - 16U, Current);
            }
            this->riffWavHeaderSize += chunkSize;
        }
        else if (0 == strncmp(chunkID, "data", 4U))
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
                this->streamBuffers[0U] = CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, MaxStreamBufferSamples * this->bytesPerSample);
                this->streamBuffers[1U] = CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, MaxStreamBufferSamples * this->bytesPerSample);
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
    this->currentStreamBufferIndex = 0U;
    this->bytesToLoadToBackBuffer = this->lengthInBytes < MaxStreamBufferSamples * this->bytesPerSample ? this->lengthInBytes : MaxStreamBufferSamples * this->bytesPerSample;
    this->seekPosition = this->bytesToLoadToBackBuffer;
    FSWrapper::Seek(this->streamFileHandle, this->riffWavHeaderSize, Begin);
    FSWrapper::Read(this->streamFileHandle, this->streamBuffers[0U], this->bytesToLoadToBackBuffer);
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

        FSWrapper::Read(this->streamFileHandle, this->streamBuffers[this->currentStreamBufferIndex ^ 1U], bytesToLoadThisFrame);
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
    this->currentStreamBufferIndex ^= 1U;
    this->backBufferFilled = false;
    if (this->seekPosition >= this->lengthInBytes)
    {
        this->endOfStream = true;
    }
}

//------------------------------------------------------------------------------
/**
*/
void* const
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

//------------------------------------------------------------------------------
/**
*/
void* const
WavCodec::FillStreamBuffer(unsigned int numSamples, unsigned int position)
{
    unsigned int desiredSeekPosition = position * this->bytesPerSample;
    if (desiredSeekPosition < this->lengthInBytes)
    {
        if (this->seekPosition != desiredSeekPosition)
        {
            FSWrapper::Seek(this->streamFileHandle, desiredSeekPosition + this->riffWavHeaderSize, Begin);
            this->seekPosition = desiredSeekPosition;
        }
        unsigned int bytesToLoad = numSamples * this->bytesPerSample;
        if ((this->seekPosition + bytesToLoad) > this->lengthInBytes)
        {
            bytesToLoad = this->lengthInBytes - seekPosition;
        }
        this->currentStreamBufferIndex ^= 1U;
        FSWrapper::Read(this->streamFileHandle, this->streamBuffers[this->currentStreamBufferIndex], bytesToLoad);
        this->seekPosition += bytesToLoad;
    }
    return this->streamBuffers[this->currentStreamBufferIndex];
}

} // namespace audio
} // namespace chrissly