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
    virtual bool SetupSound(const char* name, Mode mode, void** sampleBuffer, unsigned int& length, AudioFormat& format, SoundType& type, int& channels, int& bits) = 0;
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
