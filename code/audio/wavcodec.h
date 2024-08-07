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
    void SetupSound(const char* const filename, Mode mode, void** sampleBuffer, unsigned int& length, AudioFormat& format, SoundType& type, int& channels, int& bits, unsigned int& sampleRate);
    /// initialise the codec for streaming
    void InitialiseStream();
    /// loads data into the current streambackbuffer
    void FillStreamBackBuffer();
    /// swaps the streambuffers, called when a buffer finishes playing
    void SwapStreamBuffers();
    /// get pointer to the current streambuffer
    void* GetStreamBufferPointer() const;
    /// retrieves the length of the current streambuffer
    unsigned int GetStreamBufferLength() const;
    /// gets a value that indicates whether the current stream position is at the end of the stream
    bool EndOfStream() const;
    /// loads the given number of samples at the given position of the stream into the current streambuffer and returns a pointer to it
    void* FillStreamBuffer(unsigned int numSamples, unsigned int position);

private:
    /// copy constructor
    WavCodec(const WavCodec&);
    /// prevent copy by assignment
    WavCodec& operator = (const WavCodec&);

    core::FileHandle streamFileHandle;
    mutable void* streamBuffers[2U];
    unsigned int dataChunkOffset;
    unsigned int lengthInBytes;
    unsigned int seekPosition;
    unsigned int currentStreamBufferLength;
    unsigned int bytesToLoadToBackBuffer;
    unsigned char currentStreamBufferIndex;
    unsigned char bytesPerSample;
    bool openedAsStream;
    bool backBufferFilled;
    bool endOfStream;
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
