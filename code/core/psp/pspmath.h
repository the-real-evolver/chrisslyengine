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

//------------------------------------------------------------------------------
namespace chrissly 
{
class PSPMath
{
public:
    /// returns the squareroot of the given value
    static inline float Sqrt(float fValue) {return pspFpuSqrt(fValue);}
    /// returns the sine of the given value
    static inline float Sin(float fValue) {return pspFpuSin(fValue);}
    /// returns the cosine of the given value
    static inline float Cos(float fValue) {return pspFpuCos(fValue);}
    /// returns the arc tangent of the given value
    static inline float ATan(float fValue) {return pspFpuAtan(fValue);}
    /// returns the floating-point remainder of numerator / denominator
    static inline float Fmod(float fNumerator, float fDenominator) {return pspFpuFmod(fNumerator, fDenominator);}
};
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
