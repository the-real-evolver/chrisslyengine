//------------------------------------------------------------------------------
//  pspinput.cpp
//  (C) 2022 Christian Bleicher
//------------------------------------------------------------------------------
#include "chrisslyinput.h"
#include "common/commoninput.h"
#include "chrisslymath.h"
#include <pspkernel.h>
#include <pspctrl.h>
#include <string.h>

static const float PSP_ANALOGUE_STICK_DEADZONE = 0.2f;

//------------------------------------------------------------------------------
/**
*/
void
ce_input_initialise()
{
    sceCtrlSetSamplingCycle(100);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    sceKernelDcacheWritebackAll();
}

//------------------------------------------------------------------------------
/**
*/
void
ce_input_shutdown()
{

}

//------------------------------------------------------------------------------
/**
*/
bool
ce_input_gamepad_connected()
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ce_input_gamepad_get_state(ce_gamepad_state* const gps)
{
    memset(gps, 0, sizeof(*gps));
    SceCtrlData gamepad_state = {0U, 0U, 0U, 0U, {0U}};
    sceCtrlReadBufferPositive(&gamepad_state, 1);

    // Buttons
    if (gamepad_state.Buttons & PSP_CTRL_UP) gps->buttons |= GAMEPAD_DPAD_UP;
    if (gamepad_state.Buttons & PSP_CTRL_DOWN) gps->buttons |= GAMEPAD_DPAD_DOWN;
    if (gamepad_state.Buttons & PSP_CTRL_LEFT) gps->buttons |= GAMEPAD_DPAD_LEFT;
    if (gamepad_state.Buttons & PSP_CTRL_RIGHT) gps->buttons |= GAMEPAD_DPAD_RIGHT;
    if (gamepad_state.Buttons & PSP_CTRL_START) gps->buttons |= GAMEPAD_START;
    if (gamepad_state.Buttons & PSP_CTRL_SELECT) gps->buttons |= GAMEPAD_SELECT;
    if (gamepad_state.Buttons & PSP_CTRL_LTRIGGER) gps->buttons |= GAMEPAD_LEFT_SHOULDER;
    if (gamepad_state.Buttons & PSP_CTRL_RTRIGGER) gps->buttons |= GAMEPAD_RIGHT_SHOULDER;
    if (gamepad_state.Buttons & PSP_CTRL_HOME) gps->buttons |= GAMEPAD_HOME;
    if (gamepad_state.Buttons & PSP_CTRL_CROSS) gps->buttons |= GAMEPAD_CROSS;
    if (gamepad_state.Buttons & PSP_CTRL_CIRCLE) gps->buttons |= GAMEPAD_CIRCLE;
    if (gamepad_state.Buttons & PSP_CTRL_SQUARE) gps->buttons |= GAMEPAD_SQUARE;
    if (gamepad_state.Buttons & PSP_CTRL_TRIANGLE) gps->buttons |= GAMEPAD_TRIANGLE;
    if (gamepad_state.Buttons & PSP_CTRL_NOTE) gps->buttons |= GAMEPAD_NOTE;
    if (gamepad_state.Buttons & PSP_CTRL_SCREEN) gps->buttons |= GAMEPAD_SCREEN;
    if (gamepad_state.Buttons & PSP_CTRL_VOLUP) gps->buttons |= GAMEPAD_VOLUP;
    if (gamepad_state.Buttons & PSP_CTRL_VOLDOWN) gps->buttons |= GAMEPAD_VOLDOWN;

    // Analog stick
    ce_input_filter_dead_zone((gamepad_state.Lx - 128) / 127.0f, (gamepad_state.Ly - 128) / 127.0f, &gps->left_stick_x, &gps->left_stick_y, PSP_ANALOGUE_STICK_DEADZONE, 1.0f);
}
