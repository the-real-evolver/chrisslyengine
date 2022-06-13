#ifndef CHRISSLYINPUTPLATFORM_H_
#define CHRISSLYINPUTPLATFORM_H_
//------------------------------------------------------------------------------
/**
    @file input/chrisslyinputplatform.h

    Platform specific functions.

    (C) 2022 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __CE_WINDOWS__
#include <windows.h>

/// processes windows messages and updates states of the input devices
void ce_input_wnd_proc_handler(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);
//------------------------------------------------------------------------------
#endif
#endif