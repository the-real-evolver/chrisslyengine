#ifndef FSWRAPPER_H_
#define FSWRAPPER_H_
//------------------------------------------------------------------------------
/**
    Wrapper for the filesystem.

    (C) 2011 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __PSP__
#include "psp/pspfswrapper.h"
namespace chrissly
{
namespace core
{

typedef chrissly::PSPFSWrapper FSWrapper;

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __ANDROID__
#include "android/androidfswrapper.h"
namespace chrissly
{
namespace core
{

typedef chrissly::AndroidFSWrapper FSWrapper;

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "FSWrapper class not implemented on this platform!"
#endif
#endif
