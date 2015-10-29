#ifndef CHRISSLYMATH_H_
#define CHRISSLYMATH_H_
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
#include "android/standardmath.h"
namespace chrissly
{
namespace core
{

typedef chrissly::StandardMath Math;

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "Math class not implemented on this platform!"
#endif
#endif
