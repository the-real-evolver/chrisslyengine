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
class DSP;

/// this struct is used when creating a dsp
struct DspDescription
{
    /// callback called when the dsp is created
    Result (*setup)(DSP* const dsp);
    /// callback called when the dsp is released
    Result (*release)(DSP* const dsp);
    /// callback where processing is done
    Result (*process)(DSP* const dsp, int numChannels, int bits, unsigned int numSamples, const void* const inBuffer, void* const outBuffer);
    /// number of parameters of this dsp
    int numParameters;
    /// callback called when a float parameter is altered
    Result (*setParamFloat)(DSP* const dsp, int idx, float value);
    /// optional data supplied by the user, will be passed to the Process callback function during playback
    void* userData;
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
    Result GetBypass(bool* const enabled) const;
    /// retrieves the number of parameters of the dsp
    Result GetNumParameters(int* const numParams) const;
    /// sets a floating-point parameter to the dsp
    Result SetParameterFloat(int idx, float value);
    /// sets the user data
    Result SetUserData(void* const data);
    /// retrieves the user data
    Result GetUserData(void** const data);
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
    Result (*release)(DSP* const dsp);
    Result (*process)(DSP* const dsp, int numChannels, int bits, unsigned int numSamples, const void* const inbuffer, void* const outbuffer);
    int numParameters;
    Result (*setParamFloat)(DSP* const dsp, int idx, float value);
    void* userData;
    int buffer[1024U];
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
