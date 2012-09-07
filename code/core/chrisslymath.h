#ifndef MATH_H_
#define MATH_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::core::Math
        
    (C) 2012 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __PSP__
#include "psp/pspmath.h"
namespace chrissly
{
namespace core
{

typedef chrissly::PSPMath Math;

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __ANDROID__
#include "android/androidmath.h"
namespace chrissly
{
namespace core
{

typedef chrissly::AndroidMath Math;

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "Math class not implemented on this platform!"
#endif
#endif
