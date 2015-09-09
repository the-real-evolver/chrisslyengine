#ifndef PSPMATH_H_
#define PSPMATH_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::PSPMath

    Wrapper class for common mathematical functions using the FPU of the
    Playstation Portable. 

    (C) 2012 Christian Bleicher
*/
#include <pspfpu.h>
#include <math.h>
#include <limits.h>

//------------------------------------------------------------------------------
namespace chrissly 
{
class PSPMath
{
public:
    /// returns base raised to the power exponent
    static inline float Pow(float base, float exponent);
    /// returns the squareroot of the given value
    static inline float Sqrt(float fValue);
    /// returns the sine of the given value
    static inline float Sin(float fValue);
    /// returns the cosine of the given value
    static inline float Cos(float fValue);
    /// returns the arc tangent of the given value
    static inline float ATan(float fValue);
    /// returns the principal value of the arc tangent of [y, x] in radians
    static inline float ATan2(float y, float x);
    /// returns the floating-point remainder of numerator / denominator
    static inline float Fmod(float fNumerator, float fDenominator);
    /// rounds the value downward, returning the largest integral value that is not greater than value
    static inline float Floor(float fValue);
    /// computes the absolute value of an integer number
    static inline int Abs(int n);
};

//------------------------------------------------------------------------------
/**
*/
inline float
PSPMath::Pow(float base, float exponent)
{
    return pow(base, exponent);
}

//------------------------------------------------------------------------------
/**
*/
inline float
PSPMath::Sqrt(float fValue)
{
    return pspFpuSqrt(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline float
PSPMath::Sin(float fValue)
{
    return pspFpuSin(fValue);
}

 //------------------------------------------------------------------------------
/**
*/
inline float
PSPMath::Cos(float fValue)
{
    return pspFpuCos(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline float
PSPMath::ATan(float fValue)
{
    return pspFpuAtan(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline float
PSPMath::ATan2(float y, float x)
{
    return atan2(y, x);
}

//------------------------------------------------------------------------------
/**
*/
inline float
PSPMath::Fmod(float fNumerator, float fDenominator)
{
    return pspFpuFmod(fNumerator, fDenominator);
}

//------------------------------------------------------------------------------
/**
*/
inline float
PSPMath::Floor(float fValue)
{
    return pspFpuFloor(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline int
PSPMath::Abs(int n)
{
    int mask = (n >> (sizeof(int) * CHAR_BIT - 1));
    return (n + mask) ^ mask;
}

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
