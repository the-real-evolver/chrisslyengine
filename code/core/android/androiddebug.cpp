//------------------------------------------------------------------------------
//  androiddebug.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "androiddebug.h"
#include <android/log.h>

void ce_android_assert(bool exp, char* msg)
{
    if (!exp)
    {
        // Log an assertion failure and SIGTRAP the process to have a chance
        // to inspect it, if a debugger is attached. This uses the FATAL priority.
        __android_log_assert("assert", "chrisslyengine_android", msg);
    }
}
