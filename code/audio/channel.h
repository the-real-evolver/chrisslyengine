#ifndef CHANNEL_H_
#define CHANNEL_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::Channel

    (C) 2014 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __PSP__
#include "psp/pspchannel.h"
namespace chrissly
{
namespace audio
{

typedef chrissly::PSPChannel Channel;

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __ANDROID__
#include "android/sleschannel.h"
namespace chrissly
{
namespace audio
{

typedef chrissly::SLESChannel Channel;

} // namespace audio
} // namespace chrissly
  //------------------------------------------------------------------------------
#elif __WINDOWS__
#include "channelbase.h"
namespace chrissly
{
namespace audio
{

typedef ChannelBase Channel;

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "Channel class not implemented on this platform!"
#endif
#endif
