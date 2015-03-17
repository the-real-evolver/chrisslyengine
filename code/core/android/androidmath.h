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
    /// returns the squareroot of the given value
    static inline float Sqrt(float fValue);
    /// returns the sine of the given value
    static inline float Sin(float fValue);
    /// returns the cosine of the given value
    static inline float Cos(float fValue);
    /// returns the arc tangent of the given value
    static inline float ATan(float fValue);
    /// returns the floating-point remainder of numerator / denominator
    static inline float Fmod(float fNumerator, float fDenominator);
    /// rounds the value downward, returning the largest integral value that is not greater than value
    static inline float Floor(float fValue);
};

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

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
