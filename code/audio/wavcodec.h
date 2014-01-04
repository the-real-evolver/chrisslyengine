#ifndef WAVCODEC_H_
#define WAVCODEC_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::WavCodec

    (C) 2014 Christian Bleicher
*/
#include "codec.h"

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
    bool SetupSound(const char* filename, Mode mode, void** sampleBuffer, unsigned int& length, AudioFormat& format, int& channels, int& bits);
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
