//------------------------------------------------------------------------------
//  androiddebug.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "androiddebug.h"
#include <android/log.h>
#include <stdio.h>

//------------------------------------------------------------------------------
/**
*/
void
ce_android_assert(bool cond, const char* const exp, const char* const file, int line, const char* const msg, ...)
{
    if (!cond)
    {
        char message[128U];
        va_list args;
        va_start(args, msg);
        vsnprintf(message, 128U, msg, args);
        va_end(args);
        char output[256U];
        snprintf(output, 256U, "Assertion: '%s' in file '%s' at line %i, %s\n", exp, file, line, message);
        // Log an assertion failure and SIGTRAP the process to have a chance
        // to inspect it, if a debugger is attached. This uses the FATAL priority.
        __android_log_assert("assert", "chrisslyengine_android", "%s", output);
    }
}