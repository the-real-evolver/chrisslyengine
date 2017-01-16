#ifndef VORBISCODEC_H_
#define VORBISCODEC_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::VorbisCodec

    (C) 2014 Christian Bleicher
*/
#include "codec.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace audio
{

class VorbisCodec : public Codec
{
public:
    /// default constructor
    VorbisCodec();
    /// destructor
    ~VorbisCodec();
    /// called by Sound, create buffers ect.
    void SetupSound(const char* filename, Mode mode, void** sampleBuffer, unsigned int& length, AudioFormat& format, SoundType& type, int& channels, int& bits, unsigned int& sampleRate);
    /// initialise the codec for streaming
    void InitialiseStream();
    /// loads data into the current streambackbuffer
    void FillStreamBackBuffer();
    /// loads the given number of samples at the given position of the stream into the current streambuffer and returns a pointer to it
    void* FillStreamBuffer(unsigned int numSamples, unsigned int position);
    /// swaps the streambuffers, called when a buffer finishes playing
    void SwapStreamBuffers();
    /// get pointer to the current streambuffer
    void* GetStreamBufferPointer() const;
    /// retrieves the length of the current streambuffer
    unsigned int GetStreamBufferLength() const;
    /// gets a value that indicates whether the current stream position is at the end of the stream
    bool EndOfStream() const;
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
