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
        char message[128];
        va_list args;
        va_start(args, msg);
        vsnprintf(message, 128, msg, args);
        va_end(args);
        char output[256];
        snprintf(output, 256, "Assertion: '%s' in file '%s' at line %i, %s\n", exp, file, line, message);
        pspDebugScreenInit();
        pspDebugScreenClear();
        pspDebugScreenSetTextColor(0x000077ff);
        pspDebugScreenSetXY(0, 0);
        pspDebugScreenPrintf(output);
        sceDisplayWaitVblankStart();
        assert(0);
    }
}