#ifndef ANDROIDMATH_H_
#define ANDROIDMATH_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::AndroidMath

    Wrapper class for common mathematical functions.

    (C) 2012 Christian Bleicher
*/
#include <math.h>

//------------------------------------------------------------------------------
namespace chrissly 
{
class AndroidMath
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
AndroidMath::Pow(float base, float exponent)
{
    return pow(base, exponent);
}

//------------------------------------------------------------------------------
/**
*/
inline float
AndroidMath::Sqrt(float fValue)
{
    return sqrt(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline float
AndroidMath::Sin(float fValue)
{
    return sin(fValue);
}

 //------------------------------------------------------------------------------
/**
*/
inline float
AndroidMath::Cos(float fValue)
{
    return cos(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline float
AndroidMath::ATan(float fValue)
{
    return atan(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline float
AndroidMath::ATan2(float y, float x)
{
    return atan2(y, x);
}

//------------------------------------------------------------------------------
/**
*/
inline float
AndroidMath::Fmod(float fNumerator, float fDenominator)
{
    return fmod(fNumerator, fDenominator);
}

//------------------------------------------------------------------------------
/**
*/
inline float
AndroidMath::Floor(float fValue)
{
    return floor(fValue);
}

//------------------------------------------------------------------------------
/**
*/
inline int
AndroidMath::Abs(int n)
{
    int mask = (n >> (sizeof(int) * CHAR_BIT - 1));
    return (n + mask) ^ mask;
}

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
