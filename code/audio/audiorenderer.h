#ifndef AUDIORENDERER_H_
#define AUDIORENDERER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::AudioRenderer

    (C) 2014 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __PSP__
#include "psp/pspaudiorenderer.h"
namespace chrissly
{
namespace audio
{

typedef chrissly::PSPAudioRenderer AudioRenderer;

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __ANDROID__
#include "android/slesaudiorenderer.h"
namespace chrissly
{
namespace audio
{

typedef chrissly::SLESAudioRenderer AudioRenderer;

} // namespace audio
} // namespace chrissly
  //------------------------------------------------------------------------------
#elif __WINDOWS__
#include "windows/wasapiaudiorenderer.h"
namespace chrissly
{
namespace audio
{

typedef chrissly::WASAPIAudioRenderer AudioRenderer;

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "AudioRenderer class not implemented on this platform!"
#endif
#endif
