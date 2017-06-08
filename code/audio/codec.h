#ifndef CODEC_H_
#define CODEC_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::Codec

    (C) 2014 Christian Bleicher
*/
#include "modeflags.h"
#include "audioformat.h"
#include "soundtype.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace audio
{

class Codec
{
public:
    /// default constructor
    Codec() {};
    /// destructor
    virtual ~Codec() {};
    /// called by Sound, create buffers ect.
    virtual void SetupSound(const char* const name, Mode mode, void** sampleBuffer, unsigned int& length, AudioFormat& format, SoundType& type, int& channels, int& bits, unsigned int& sampleRate) = 0;
    /// initialise the codec for streaming
    virtual void InitialiseStream() = 0;
    /// loads data into the current streambackbuffer
    virtual void FillStreamBackBuffer() = 0;
    /// swaps the streambuffers, called when a buffer finishes playing
    virtual void SwapStreamBuffers() = 0;
    /// get pointer to the current streambuffer
    virtual void* const GetStreamBufferPointer() const = 0;
    /// retrieves the length of the current streambuffer
    virtual unsigned int GetStreamBufferLength() const = 0;
    /// gets a value that indicates whether the current stream position is at the end of the stream
    virtual bool EndOfStream() const = 0;
    /// loads the given number of samples at the given position of the stream into the current streambuffer and returns a pointer to it
    virtual void* const FillStreamBuffer(unsigned int numSamples, unsigned int position) = 0;
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
