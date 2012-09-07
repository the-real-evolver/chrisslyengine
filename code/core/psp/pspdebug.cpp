//------------------------------------------------------------------------------
//  pspdebug.cc
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "pspdebug.h"
#include <assert.h>
#include <pspdebug.h>
#include <pspdisplay.h>

void ce_psp_assert(bool exp, char* msg)
{
    if (!exp)
    { 
        pspDebugScreenInit();
        pspDebugScreenClear();
        pspDebugScreenSetTextColor(0x000077ff);
        pspDebugScreenSetXY(0, 0);
        pspDebugScreenPrintf(msg);
        sceDisplayWaitVblankStart();
        assert(false);
    }
}