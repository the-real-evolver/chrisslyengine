#ifndef CHRISSLYMATH_H_
#define CHRISSLYMATH_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::core::Math

    (C) 2012 Christian Bleicher
*/

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
