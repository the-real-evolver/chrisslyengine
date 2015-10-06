#ifndef CHRISSLYMUTEX_H_
#define CHRISSLYMUTEX_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::core::Mutex

    (C) 2015 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __PSP__
#include "psp/pspmutex.h"
namespace chrissly
{
namespace core
{

typedef chrissly::PSPMutex Mutex;

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __ANDROID__
#include "android/posixmutex.h"
namespace chrissly
{
namespace core
{

typedef chrissly::PosixMutex Mutex;

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "Mutex class not implemented on this platform!"
#endif
#endif
