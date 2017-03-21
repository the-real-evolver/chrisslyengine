//------------------------------------------------------------------------------
//  pspdebug.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "pspdebug.h"
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <pspdebug.h>
#include <pspdisplay.h>

//------------------------------------------------------------------------------
/**
*/
void
ce_psp_assert(bool cond, const char* exp, const char* file, int line, const char* msg, ...)
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
        pspDebugScreenInit();
        pspDebugScreenClear();
        pspDebugScreenSetTextColor(0x000077ff);
        pspDebugScreenSetXY(0, 0);
        pspDebugScreenPrintf(output);
        sceDisplayWaitVblankStart();
        assert(0);
    }
}