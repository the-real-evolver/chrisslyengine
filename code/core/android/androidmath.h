#ifndef ANDROIDMATH_H_
#define ANDROIDMATH_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::core::AndroidMath
    
    Wrapper class for common mathematical functions
    
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
    static inline float Sqrt(float fValue) {return sqrt(fValue);}
    /// returns the sine of the given value
    static inline float Sin(float fValue) {return sin(fValue);}
    /// returns the cosine of the given value
    static inline float Cos(float fValue) {return cos(fValue);}
    /// returns the arc tangent of the given value
    static inline float ATan(float fValue) {return atan(fValue);}
    /// returns the floating-point remainder of numerator / denominator
    static inline float Fmod(float fNumerator, float fDenominator) {return fmod(fNumerator, fDenominator);}
};
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
