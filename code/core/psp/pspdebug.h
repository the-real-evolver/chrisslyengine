//------------------------------------------------------------------------------
/**
    @file core/psp/pspdebug.h

    (C) 2012 Christian Bleicher
*/

/// display assertion message in the debugger and assert if cond is false
void ce_psp_assert(bool cond, const char* const exp, const char* const file, int line, const char* const msg, ...);
