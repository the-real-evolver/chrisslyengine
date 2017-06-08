//------------------------------------------------------------------------------
/**
    @file core/windows/winapidebug.h

    (C) 2016 Christian Bleicher
*/

/// display assertion message in the debugger and assert if cond is false
void ce_windows_assert(bool cond, const char* const exp, const char* const file, int line, const char* const msg, ...);
/// formats the C string pointed by msg and sends it to the debugger for display
void ce_windows_log(const char* const msg, ...);
