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
    void SetupSound(const char* filename, Mode mode, void** sampleBuffer, unsigned int& length, AudioFormat& format, SoundType& type, int& channels, int& bits);
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
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
