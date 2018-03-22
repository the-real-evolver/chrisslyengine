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
    /// returns the tangent of the given value (measured in radians)
    static inline float Tan(float fValue);
    /// returns the arc tangent of the given value
    static inline float ATan(float fValue);
    /// returns the principal value of the arc tangent of [y, x] in radians
    static inline float ATan2(float y, float x);
    /// returns the floating-point remainder of numerator / denominator
    static inline float Fmod(float fNumerator, float fDenominator);
    /// rounds the value downward, returning the largest integral value that is not greater than value
    static inline float Floor(float fValue);
    /// rounds the value upward, returning the smallest integral value that is not less than value
    static inline float Ceil(float fValue);
    /// computes the absolute value of an floating point number
    static inline float Fabs(float fValue);
    /// computes the absolute value of an integer number
    static inline int Abs(int n);
};

//------------------------------------------------------------------------------
/**
*/
inline float
PSPMath::Pow(float base, float exponent)
{
    return powf(base, exponent);
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
    float sine;
    __asm__ volatile (
        "mtv %1, S000\n"
        "vcst.s S001, VFPU_2_PI\n"
        "vmul.s S000, S000, S001\n"
        "vsin.s S000, S000\n"
        "mfv %0, S000\n"
        : "=r"(sine)
        : "r"(fValue)
    );
    return sine;
}

//------------------------------------------------------------------------------
/**
*/
inline float
PSPMath::Cos(float fValue)
{
    float cosine;
    __asm__ volatile (
        "mtv %1, S000\n"
        "vcst.s S001, VFPU_2_PI\n"
        "vmul.s S000, S000, S001\n"
        "vcos.s S000, S000\n"
        "mfv %0, S000\n"
        : "=r"(cosine)
        : "r"(fValue)
    );
    return cosine;
}

//------------------------------------------------------------------------------
/**
*/
inline float
PSPMath::Tan(float fValue)
{
    return tanf(fValue);
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
    return atan2f(y, x);
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
inline float
PSPMath::Ceil(float fValue)
{
    return pspFpuCeil(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline float
PSPMath::Fabs(float fValue)
{
    return pspFpuAbs(fValue);
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
