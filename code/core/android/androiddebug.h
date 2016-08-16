//------------------------------------------------------------------------------
/**
    @file core/android/androiddebug.h

    (C) 2012 Christian Bleicher
*/

/// display assertion message in the debugger and assert if cond is false
void ce_android_assert(bool cond, const char* exp, const char* file, int line, const char* msg, ...);