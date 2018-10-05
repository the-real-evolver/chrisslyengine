#ifndef DSP_H_
#define DSP_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::DSP

    (C) 2018 Christian Bleicher
*/
#include "errorcode.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace audio
{

/// this struct is used when creating a dsp
struct DspDescription
{
    /// callback where processing is done
    Result (*Process)(int numChannels, int bits, unsigned int numSamples, void* inBuffer, void* outBuffer);
};

class DSP
{
    friend class AudioSystem;
    friend class ChannelBase;
public:
    /// default constructor
    DSP();
    /// destructor
    ~DSP();
    /// enables or disables the process callback of the dsp
    Result SetBypass(bool enabled);
    /// retrieves the bypass state of the dsp
    Result GetBypass(bool* const enabled);
    /// frees the dsp object and makes it available for further use
    Result Release();

private:
    /// copy constructor
    DSP(const DSP&);
    /// prevent copy by assignment
    DSP& operator = (const DSP&);
    /// initialise dsp object
    void Setup(const DspDescription* const desc);
    /// returns if the dsp is setup
    bool IsInUse() const;

    bool bypass;
    bool inUse;
    Result (*Process)(int numChannels, int bits, unsigned int numSamples, void* inbuffer, void* outbuffer);
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
