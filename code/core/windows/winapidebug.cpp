//------------------------------------------------------------------------------
//  winapidebug.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "winapidebug.h"
#include <stdio.h>
#include <assert.h>
#include <windows.h>
#include <Strsafe.h>

//------------------------------------------------------------------------------
/**
*/
void
ce_windows_assert(bool cond, const char* exp, const char* file, int line, const char* msg, ...)
{
    if (!cond)
    {
        char message[128U];
        va_list args;
        va_start(args, msg);
        StringCchVPrintf(message, 128U, msg, args);
        va_end(args);
        ce_windows_log("Assertion: '%s' in file '%s' at line %i, %s\n", exp, file, line, message);
        assert(0);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ce_windows_log(const char* msg, ...)
{
    char output[256U];
    va_list args;
    va_start(args, msg);
    StringCchVPrintf(output, 256U, msg, args);
    va_end(args);
    OutputDebugString(output);
}
