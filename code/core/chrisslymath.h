#ifndef CHRISSLYMATH_H_
#define CHRISSLYMATH_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::core::Math

    (C) 2012 Christian Bleicher
*/

//------------------------------------------------------------------------------
/// computes a + t * (b - a), i.e. the linear interpolation between a and b for the parameter t (or extrapolation, when t is outside the range [0,1])
inline float ce_math_lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

//------------------------------------------------------------------------------
#if __CE_PSP__
#include "psp/pspmath.h"
namespace chrissly
{
namespace core
{

typedef chrissly::PSPMath Math;

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __CE_ANDROID__ || __CE_WINDOWS__
#include "standard/standardmath.h"
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
