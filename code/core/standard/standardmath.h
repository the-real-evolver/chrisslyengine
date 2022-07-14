#ifndef STANDARDMATH_H_
#define STANDARDMATH_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::StandardMath

    Wrapper class for common mathematical functions.

    (C) 2012 Christian Bleicher
*/
#define _USE_MATH_DEFINES
#include <math.h>
#include <limits.h>

//------------------------------------------------------------------------------
namespace chrissly
{
class StandardMath
{
public:
    /// returns base raised to the power exponent
    static inline float Pow(float base, float exponent);
    /// returns the squareroot of the given value
    static inline float Sqrt(float fValue);
    /// returns the sine of the given value
    static inline float Sin(float fValue);
    /// returns the arc sine of the given value
    static inline float ASin(float fValue);
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
StandardMath::Pow(float base, float exponent)
{
    return powf(base, exponent);
}

//------------------------------------------------------------------------------
/**
*/
inline float
StandardMath::Sqrt(float fValue)
{
    return sqrtf(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline float
StandardMath::Sin(float fValue)
{
    return sinf(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline float
StandardMath::ASin(float fValue)
{
    return asinf(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline float
StandardMath::Cos(float fValue)
{
    return cosf(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline float
StandardMath::Tan(float fValue)
{
    return tanf(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline float
StandardMath::ATan(float fValue)
{
    return atanf(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline float
StandardMath::ATan2(float y, float x)
{
    return atan2f(y, x);
}

//------------------------------------------------------------------------------
/**
*/
inline float
StandardMath::Fmod(float fNumerator, float fDenominator)
{
    return fmodf(fNumerator, fDenominator);
}

//------------------------------------------------------------------------------
/**
*/
inline float
StandardMath::Floor(float fValue)
{
    return floorf(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline float
StandardMath::Ceil(float fValue)
{
    return ceilf(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline float
StandardMath::Fabs(float fValue)
{
    return fabsf(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline int
StandardMath::Abs(int n)
{
    int mask = (n >> (sizeof(int) * CHAR_BIT - 1));
    return (n + mask) ^ mask;
}

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
