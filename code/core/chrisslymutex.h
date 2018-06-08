#ifndef CHRISSLYMUTEX_H_
#define CHRISSLYMUTEX_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::core::Mutex

    (C) 2015 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __CE_PSP__
#include "psp/pspmutex.h"
namespace chrissly
{
namespace core
{

typedef chrissly::PSPMutex Mutex;

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __CE_ANDROID__
#include "posix/posixmutex.h"
namespace chrissly
{
namespace core
{

typedef chrissly::PosixMutex Mutex;

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __CE_WINDOWS__
#include "windows/winapimutex.h"
namespace chrissly
{
namespace core
{

typedef chrissly::WinAPIMutex Mutex;

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "Mutex class not implemented on this platform!"
#endif
#endif
