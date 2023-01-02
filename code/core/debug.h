#ifndef DEBUG_H_
#define DEBUG_H_
//------------------------------------------------------------------------------
/**
    @file core/debug.h

    Chrissly Engine debug macros.

    (C) 2011 Christian Bleicher
*/

#if __CE_DEBUG__
//------------------------------------------------------------------------------
#if __CE_PSP__
#include "psp/pspdebug.h"
#define CE_ASSERT(exp, ...) ce_psp_assert(exp, #exp, __FILE__, __LINE__, __VA_ARGS__)
#define CE_LOG(...)
//------------------------------------------------------------------------------
#elif __CE_ANDROID__
#include "android/androiddebug.h"
#include <android/log.h>
#define CE_ASSERT(exp, ...) ce_android_assert(exp, #exp, __FILE__, __LINE__, __VA_ARGS__)
#define CE_LOG(...) __android_log_print(ANDROID_LOG_INFO, "chrisslyengine_android", __VA_ARGS__)
//------------------------------------------------------------------------------
#elif __CE_WINDOWS__
#include "windows/winapidebug.h"
#define CE_ASSERT(exp, ...) ce_windows_assert(exp, #exp, __FILE__, __LINE__, __VA_ARGS__)
#define CE_LOG(...) ce_windows_log(__VA_ARGS__)
//------------------------------------------------------------------------------
#else
#error "Debug macros not implemented on this platform!"
#endif
#else
#define CE_ASSERT(exp, ...)
#define CE_LOG(...)
#endif

#endif