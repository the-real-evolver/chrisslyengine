#ifndef VORBISCODEC_H_
#define VORBISCODEC_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::VorbisCodec

    (C) 2014 Christian Bleicher
*/
#include "codec.h"
#include "stb_vorbis.h"

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
    void SetupSound(const char* const filename, Mode mode, void** sampleBuffer, unsigned int& length, AudioFormat& format, SoundType& type, int& channels, int& bits, unsigned int& sampleRate);
    /// initialise the codec for streaming
    void InitialiseStream();
    /// loads data into the current streambackbuffer
    void FillStreamBackBuffer();
    /// swaps the streambuffers, called when a buffer finishes playing
    void SwapStreamBuffers();
    /// get pointer to the current streambuffer
    void* const GetStreamBufferPointer() const;
    /// retrieves the length of the current streambuffer
    unsigned int GetStreamBufferLength() const;
    /// gets a value that indicates whether the current stream position is at the end of the stream
    bool EndOfStream() const;
    /// loads the given number of samples at the given position of the stream into the current streambuffer and returns a pointer to it
    void* const FillStreamBuffer(unsigned int numSamples, unsigned int position);

private:
    /// copy constructor
    VorbisCodec(const VorbisCodec&);
    /// prevent copy by assignment
    VorbisCodec& operator = (const VorbisCodec&);

    unsigned int lengthInSamples;
    bool openedAsStream;
    unsigned int seekPosition;
    mutable void* streamBuffers[2U];
    unsigned char currentStreamBufferIndex;
    void* fileBuffer;
    stb_vorbis* vorbisStream;
    stb_vorbis_info vorbisInfo;
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
