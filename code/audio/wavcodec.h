#ifndef WAVCODEC_H_
#define WAVCODEC_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::WavCodec

    (C) 2014 Christian Bleicher
*/
#include "codec.h"
#include "filehandle.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace audio
{

class WavCodec : public Codec
{
public:
    /// default constructor
    WavCodec();
    /// destructor
    ~WavCodec();
    /// called by Sound, create buffers ect.
    void SetupSound(const char* filename, Mode mode, void** sampleBuffer, unsigned int& length, AudioFormat& format, SoundType& type, int& channels, int& bits, unsigned int& sampleRate);
    /// initialise the codec for streaming
    void InitialiseStream();
    /// loads data into the current streambackbuffer
    void FillStreamBackBuffer();
    /// loads the given number of samples into the current streambuffer
    void FillStreamBuffer(unsigned int numSamples);
    /// swaps the streambuffers, called when a buffer finishes playing
    void SwapStreamBuffers();
    /// get pointer to the current streambuffer
    void* GetStreamBufferPointer() const;
    /// retrieves the length of the current streambuffer
    unsigned int GetStreamBufferLength() const;
    /// gets a value that indicates whether the current stream position is at the end of the stream
    bool EndOfStream() const;

private:
    unsigned int riffWavHeaderSize;
    unsigned int lengthInBytes;
    unsigned char bytesPerSample;
    bool openedAsStream;
    core::FileHandle streamFileHandle;
    unsigned int seekPosition;
    bool endOfStream;
    mutable void* streamBuffers[2];
    unsigned char currentStreamBufferIndex;
    unsigned int currentStreamBufferLength;
    unsigned int bytesToLoadToBackBuffer;
    bool backBufferFilled;
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
