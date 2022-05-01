//------------------------------------------------------------------------------
//  windowsinput.cpp
//  (C) 2022 Christian Bleicher
//------------------------------------------------------------------------------
#include "input/chrisslyinput.h"
#include "input/common/commoninput.h"
#include "debug.h"
#include <xinput.h>
#include <Dbt.h>

static const float ANALOG_STICK_EMULATED_DEFAULT_VALUE = 0.7f;

static ce_gamepad_state gamepad_state = {0U, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
static bool gamepad_check_availability = true;
static bool gamepad_connected = false;

//------------------------------------------------------------------------------
/**
*/
void
ce_input_wnd_proc_handler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CE_UNREFERENCED_PARAMETER(hwnd);
    CE_UNREFERENCED_PARAMETER(lParam);

    switch (msg)
    {
        case WM_DEVICECHANGE:
            if ((UINT)wParam == DBT_DEVNODES_CHANGED) gamepad_check_availability = true;
            break;
        case WM_KEYDOWN:
            switch (wParam)
            {
                case VK_LEFT:
                    gamepad_state.leftStickX = -ANALOG_STICK_EMULATED_DEFAULT_VALUE;
                    break;
                case VK_RIGHT:
                    gamepad_state.leftStickX = ANALOG_STICK_EMULATED_DEFAULT_VALUE;
                    break;
                case VK_UP:
                    gamepad_state.leftStickY = ANALOG_STICK_EMULATED_DEFAULT_VALUE;
                    break;
                case VK_DOWN:
                    gamepad_state.leftStickY = -ANALOG_STICK_EMULATED_DEFAULT_VALUE;
                    break;
            }
            break;
        case WM_KEYUP:
            switch (wParam)
            {
                case VK_LEFT:
                case VK_RIGHT:
                    gamepad_state.leftStickX = 0.0f;
                    break;
                case VK_UP:
                case VK_DOWN:
                    gamepad_state.leftStickY = 0.0f;
                    break;
            }
            break;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ce_input_initialise()
{

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
    return gamepad_connected;
}

//------------------------------------------------------------------------------
/**
*/
void
ce_input_gamepad_get_state(ce_gamepad_state* const gps)
{
    if (gamepad_check_availability)
    {
        XINPUT_CAPABILITIES caps = {};
        gamepad_connected = XInputGetCapabilities(0U, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS;
        gamepad_check_availability = false;
    }

    if (gamepad_connected)
    {
        ZeroMemory(gps, sizeof(*gps));
        XINPUT_STATE state = {};
        DWORD dwResult = XInputGetState(0U, &state);
        if (dwResult == ERROR_SUCCESS)
        {
            // Buttons
            WORD wButtons = state.Gamepad.wButtons;
            if (wButtons & XINPUT_GAMEPAD_DPAD_UP) gps->buttons |= GAMEPAD_DPAD_UP;
            if (wButtons & XINPUT_GAMEPAD_DPAD_DOWN) gps->buttons |= GAMEPAD_DPAD_DOWN;
            if (wButtons & XINPUT_GAMEPAD_DPAD_LEFT) gps->buttons |= GAMEPAD_DPAD_LEFT;
            if (wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) gps->buttons |= GAMEPAD_DPAD_RIGHT;
            if (wButtons & XINPUT_GAMEPAD_START) gps->buttons |= GAMEPAD_START;
            if (wButtons & XINPUT_GAMEPAD_BACK) gps->buttons |= GAMEPAD_BACK;
            if (wButtons & XINPUT_GAMEPAD_LEFT_THUMB) gps->buttons |= GAMEPAD_LEFT_THUMB;
            if (wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) gps->buttons |= GAMEPAD_RIGHT_THUMB;
            if (wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) gps->buttons |= GAMEPAD_LEFT_SHOULDER;
            if (wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) gps->buttons |= GAMEPAD_RIGHT_SHOULDER;
            if (wButtons & XINPUT_GAMEPAD_A) gps->buttons |= GAMEPAD_A;
            if (wButtons & XINPUT_GAMEPAD_B) gps->buttons |= GAMEPAD_B;
            if (wButtons & XINPUT_GAMEPAD_X) gps->buttons |= GAMEPAD_X;
            if (wButtons & XINPUT_GAMEPAD_Y) gps->buttons |= GAMEPAD_Y;

            // Analog sticks
            ce_input_filter_dead_zone((float)state.Gamepad.sThumbLX, (float)state.Gamepad.sThumbLY, &gps->leftStickX, &gps->leftStickY, (float)XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, 32768.0f);
            ce_input_filter_dead_zone((float)state.Gamepad.sThumbRX, (float)state.Gamepad.sThumbRY, &gps->rightStickX, &gps->rightStickY, (float)XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, 32768.0f);

            // Trigger
            ce_input_filter_dead_zone(0.0f, (float)state.Gamepad.bLeftTrigger, &gps->leftTrigger, &gps->leftTrigger, (float)XINPUT_GAMEPAD_TRIGGER_THRESHOLD, 256.0f);
            ce_input_filter_dead_zone(0.0f, (float)state.Gamepad.bRightTrigger, &gps->rightTrigger, &gps->rightTrigger, (float)XINPUT_GAMEPAD_TRIGGER_THRESHOLD, 256.0f);
        }
        else
        {
            gamepad_connected = false;
        }
    }
    else
    {
        // wip: fallback to keyboard input
        gps->leftStickX = gamepad_state.leftStickX;
        gps->leftStickY = gamepad_state.leftStickY;
    }
}
