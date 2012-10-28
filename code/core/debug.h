//------------------------------------------------------------------------------
/**
    @file core/debug.h

    Chrissly Engine debug macros.
    
    (C) 2011 Christian Bleicher
*/
#include <stdio.h>

#if __PSP__
#include "psp/pspdebug.h"
// macro to allow formatted input without having to use stdargs.h
#define CE_ASSERT(exp, ...) {char msg[128]; sprintf(msg, __VA_ARGS__); ce_psp_assert(exp, msg);}

#elif __ANDROID__
#include "android/androiddebug.h"
#include <android/log.h>
// macro to allow formatted input without having to use stdargs.h
#define CE_ASSERT(exp, ...) {char msg[128]; sprintf(msg, __VA_ARGS__); ce_android_assert(exp, msg);}
#define CE_LOG(...) __android_log_print(ANDROID_LOG_INFO, "chrisslyengine_android", __VA_ARGS__);

#else
#error "Debug macros not implemented on this platform!"
#endif

