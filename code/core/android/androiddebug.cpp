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
ce_android_assert(bool cond, const char* exp, const char* file, int line, const char* msg, ...)
{
    if (!cond)
    {
        char message[128];
        va_list args;
        va_start(args, msg);
        vsnprintf(message, 128, msg, args);
        va_end(args);
        char output[256];
        snprintf(output, 256, "Assertion: '%s' in file '%s' at line %i, %s\n", exp, file, line, message);
        // Log an assertion failure and SIGTRAP the process to have a chance
        // to inspect it, if a debugger is attached. This uses the FATAL priority.
        __android_log_assert("assert", "chrisslyengine_android", output);
    }
}