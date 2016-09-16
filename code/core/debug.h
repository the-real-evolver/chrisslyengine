//------------------------------------------------------------------------------
/**
    @file core/debug.h

    Chrissly Engine debug macros.

    (C) 2011 Christian Bleicher
*/

#if __DEBUG__
//------------------------------------------------------------------------------
#if __PSP__
#include "psp/pspdebug.h"
#define CE_ASSERT(exp, ...) ce_psp_assert(exp, #exp, __FILE__, __LINE__, __VA_ARGS__);
#define CE_LOG(...)
//------------------------------------------------------------------------------
#elif __ANDROID__
#include "android/androiddebug.h"
#include <android/log.h>
#define CE_ASSERT(exp, ...) ce_android_assert(exp, #exp, __FILE__, __LINE__, __VA_ARGS__);
#define CE_LOG(...) __android_log_print(ANDROID_LOG_INFO, "chrisslyengine_android", __VA_ARGS__);
//------------------------------------------------------------------------------
#elif __WINDOWS__
#include "windows/winapidebug.h"
#define CE_ASSERT(exp, ...) ce_windows_assert(exp, #exp, __FILE__, __LINE__, __VA_ARGS__);
#define CE_LOG(...) ce_windows_log(__VA_ARGS__);
//------------------------------------------------------------------------------
#else
#error "Debug macros not implemented on this platform!"
#endif
#else
#define CE_ASSERT(exp, ...)
#define CE_LOG(...)
#endif

#define CE_UNREFERENCED_PARAMETER(P) (P)