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
    bool SetupSound(const char* filename, Mode mode, void** sampleBuffer, unsigned int& length, AudioFormat& format, SoundType& type, int& channels, int& bits);
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
