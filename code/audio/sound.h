#ifndef SOUND_H_
#define SOUND_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::Sound

    (C) 2014 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __PSP__
#include "soundbase.h"
namespace chrissly
{
namespace audio
{

typedef SoundBase Sound;

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __ANDROID__
#include "android/slessound.h"
namespace chrissly
{
namespace audio
{

typedef chrissly::SLESSound Sound;

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "Sound class not implemented on this platform!"
#endif
#endif
