#ifndef SOUND_H_
#define SOUND_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::Sound

    (C) 2014 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __CE_PSP__
#include "soundbase.h"
namespace chrissly
{

class PSPSound : public audio::SoundBase
{
    friend class PSPAudioRenderer;
public:
    /// default constructor
    PSPSound() {};
    /// destructor
    ~PSPSound() {};
private:
    /// copy constructor
    PSPSound(const PSPSound&);
    /// prevent copy by assignment
    PSPSound& operator = (const PSPSound&);
};

namespace audio
{

typedef chrissly::PSPSound Sound;

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __CE_SLES__
#include "sles/slessound.h"
namespace chrissly
{
namespace audio
{

typedef chrissly::SLESSound Sound;

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __CE_WASAPI__
#include "soundbase.h"
namespace chrissly
{

class WASAPISound : public audio::SoundBase
{
    friend class WASAPIAudioRenderer;
public:
    /// default constructor
    WASAPISound() {};
    /// destructor
    ~WASAPISound() {};
private:
    /// copy constructor
    WASAPISound(const WASAPISound&);
    /// prevent copy by assignment
    WASAPISound& operator = (const WASAPISound&);
};

namespace audio
{

typedef chrissly::WASAPISound Sound;

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "Sound class not implemented on this platform!"
#endif
#endif
