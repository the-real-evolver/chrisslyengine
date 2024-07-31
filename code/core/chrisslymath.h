#ifndef CHRISSLYMATH_H_
#define CHRISSLYMATH_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::core::Math

    (C) 2012 Christian Bleicher
*/
#include <stdlib.h>

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

//------------------------------------------------------------------------------
/* constants */
#define CE_2_M_PI 6.28318548f

/* computes a + t * (b - a), i.e.the linear interpolation between a and b for the parameter t (or extrapolation, when t is outside the range [0, 1]) */
inline float
ce_math_lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

/* returns a floating point, pseudo-random number in the range [0, 1] */
inline float
ce_math_rand()
{
    return (float)rand() / (float)RAND_MAX;
}

/* returns value wrapped from min and max */
inline float
ce_math_wrap(float x, float min, float max)
{
    float d = max - min;
    return min + chrissly::core::Math::Fmod(d + chrissly::core::Math::Fmod(x - min, d), d);
}

/* interpolate between start and target in the given interval with ease-in and ease-out using a fixed cubic function */
inline float
ce_math_cubic_easing(float start, float target, float interval, float current_time)
{
    float t = current_time / interval;
    float r = t * t * (3.0f - 2.0f * t);
    r *= chrissly::core::Math::Fabs(target - start);
    return target < start ? start - r : start + r;
}

//------------------------------------------------------------------------------
#endif
