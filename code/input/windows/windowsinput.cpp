//------------------------------------------------------------------------------
//  windowsinput.cpp
//  (C) 2022 Christian Bleicher
//------------------------------------------------------------------------------
#include "input/chrisslyinput.h"
#include "input/common/commoninput.h"
#include "debug.h"
#include <xinput.h>
#include <Dbt.h>
#include <windowsx.h>

static const float ANALOG_STICK_EMULATED_DEFAULT_VALUE = 0.7f;

static ce_gamepad_state gamepad_state = {0U, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
static bool gamepad_check_availability = true;
static bool gamepad_connected = false;

static ce_mouse_state current_mouse_state = {0U, 0, 0, 0, 0};
static ce_mouse_state last_mouse_state = {0U, 0, 0, 0, 0};

unsigned char keyboard_state[256U] = {0U};

//------------------------------------------------------------------------------
/**
*/
void
ce_input_wnd_proc_handler(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    CE_UNREFERENCED_PARAMETER(hwnd);

    switch (msg)
    {
        case WM_DEVICECHANGE:
            if ((UINT)w_param == DBT_DEVNODES_CHANGED) gamepad_check_availability = true;
            break;
        case WM_KEYDOWN:
            switch (w_param)
            {
                case VK_BACK:       keyboard_state[KEY_BACK] = 1U; break;
                case VK_TAB:        keyboard_state[KEY_TAB] = 1U; break;
                case VK_CLEAR:      keyboard_state[KEY_CLEAR] = 1U; break;
                case VK_RETURN:     keyboard_state[KEY_RETURN] = 1U; break;
                case VK_PAUSE:      keyboard_state[KEY_PAUSE] = 1U; break;
                case VK_CAPITAL:    keyboard_state[KEY_CAPS_LOCK] = 1U; break;
                case VK_ESCAPE:     keyboard_state[KEY_ESCAPE] = 1U; break;
                case VK_LEFT:       keyboard_state[KEY_LEFT] = 1U; gamepad_state.left_stick_x = -ANALOG_STICK_EMULATED_DEFAULT_VALUE; break;
                case VK_RIGHT:      keyboard_state[KEY_RIGHT] = 1U; gamepad_state.left_stick_x = ANALOG_STICK_EMULATED_DEFAULT_VALUE; break;
                case VK_UP:         keyboard_state[KEY_UP] = 1U; gamepad_state.left_stick_y = ANALOG_STICK_EMULATED_DEFAULT_VALUE; break;
                case VK_DOWN:       keyboard_state[KEY_DOWN] = 1U; gamepad_state.left_stick_y = -ANALOG_STICK_EMULATED_DEFAULT_VALUE; break;
                case VK_SPACE:      keyboard_state[KEY_SPACE] = 1U; break;
                case VK_PRIOR:      keyboard_state[KEY_PAGE_UP] = 1U; break;
                case VK_NEXT:       keyboard_state[KEY_PAGE_DOWN] = 1U; break;
                case VK_END:        keyboard_state[KEY_END] = 1U; break;
                case VK_PRINT:      keyboard_state[KEY_PRINT] = 1U; break;
                case VK_INSERT:     keyboard_state[KEY_INSERT] = 1U; break;
                case VK_DELETE:     keyboard_state[KEY_DELETE] = 1U; break;
                // same as ASCII '0' - '9' (0x30 - 0x39)
                case 0x30:          keyboard_state[KEY_0] = 1U; break;
                case 0x31:          keyboard_state[KEY_1] = 1U; break;
                case 0x32:          keyboard_state[KEY_2] = 1U; break;
                case 0x33:          keyboard_state[KEY_3] = 1U; break;
                case 0x34:          keyboard_state[KEY_4] = 1U; break;
                case 0x35:          keyboard_state[KEY_5] = 1U; break;
                case 0x36:          keyboard_state[KEY_6] = 1U; break;
                case 0x37:          keyboard_state[KEY_7] = 1U; break;
                case 0x38:          keyboard_state[KEY_8] = 1U; break;
                case 0x39:          keyboard_state[KEY_9] = 1U; break;
                // same as ASCII 'A' - 'Z' (0x41 - 0x5A)
                case 0x41:          keyboard_state[KEY_A] = 1U; break;
                case 0x42:          keyboard_state[KEY_B] = 1U; break;
                case 0x43:          keyboard_state[KEY_C] = 1U; break;
                case 0x44:          keyboard_state[KEY_D] = 1U; break;
                case 0x45:          keyboard_state[KEY_E] = 1U; break;
                case 0x46:          keyboard_state[KEY_F] = 1U; break;
                case 0x47:          keyboard_state[KEY_G] = 1U; break;
                case 0x48:          keyboard_state[KEY_H] = 1U; break;
                case 0x49:          keyboard_state[KEY_I] = 1U; break;
                case 0x4a:          keyboard_state[KEY_J] = 1U; break;
                case 0x4b:          keyboard_state[KEY_K] = 1U; break;
                case 0x4c:          keyboard_state[KEY_L] = 1U; break;
                case 0x4d:          keyboard_state[KEY_M] = 1U; break;
                case 0x4e:          keyboard_state[KEY_N] = 1U; break;
                case 0x4f:          keyboard_state[KEY_O] = 1U; break;
                case 0x50:          keyboard_state[KEY_P] = 1U; break;
                case 0x51:          keyboard_state[KEY_Q] = 1U; break;
                case 0x52:          keyboard_state[KEY_R] = 1U; break;
                case 0x53:          keyboard_state[KEY_S] = 1U; break;
                case 0x54:          keyboard_state[KEY_T] = 1U; break;
                case 0x55:          keyboard_state[KEY_U] = 1U; break;
                case 0x56:          keyboard_state[KEY_V] = 1U; break;
                case 0x57:          keyboard_state[KEY_W] = 1U; break;
                case 0x58:          keyboard_state[KEY_X] = 1U; break;
                case 0x59:          keyboard_state[KEY_Y] = 1U; break;
                case 0x5a:          keyboard_state[KEY_Z] = 1U; break;
                case VK_LWIN:       keyboard_state[KEY_LWIN] = 1U; break;
                case VK_RWIN:       keyboard_state[KEY_RWIN] = 1U; break;
                case VK_APPS:       keyboard_state[KEY_APPS] = 1U; break;
                case VK_NUMPAD0:    keyboard_state[KEY_NUMPAD0] = 1U; break;
                case VK_NUMPAD1:    keyboard_state[KEY_NUMPAD1] = 1U; break;
                case VK_NUMPAD2:    keyboard_state[KEY_NUMPAD2] = 1U; break;
                case VK_NUMPAD3:    keyboard_state[KEY_NUMPAD3] = 1U; break;
                case VK_NUMPAD4:    keyboard_state[KEY_NUMPAD4] = 1U; break;
                case VK_NUMPAD5:    keyboard_state[KEY_NUMPAD5] = 1U; break;
                case VK_NUMPAD6:    keyboard_state[KEY_NUMPAD6] = 1U; break;
                case VK_NUMPAD7:    keyboard_state[KEY_NUMPAD7] = 1U; break;
                case VK_NUMPAD8:    keyboard_state[KEY_NUMPAD8] = 1U; break;
                case VK_NUMPAD9:    keyboard_state[KEY_NUMPAD9] = 1U; break;
                case VK_MULTIPLY:   keyboard_state[KEY_MULTIPLY] = 1U; break;
                case VK_ADD:        keyboard_state[KEY_ADD] = 1U; break;
                case VK_SEPARATOR:  keyboard_state[KEY_SEPARATOR] = 1U; break;
                case VK_SUBTRACT:   keyboard_state[KEY_SUBTRACT] = 1U; break;
                case VK_DECIMAL:    keyboard_state[KEY_DECIMAL] = 1U; break;
                case VK_DIVIDE:     keyboard_state[KEY_DIVIDE] = 1U; break;
                case VK_F1:         keyboard_state[KEY_F1] = 1U; break;
                case VK_F2:         keyboard_state[KEY_F2] = 1U; break;
                case VK_F3:         keyboard_state[KEY_F3] = 1U; break;
                case VK_F4:         keyboard_state[KEY_F4] = 1U; break;
                case VK_F5:         keyboard_state[KEY_F5] = 1U; break;
                case VK_F6:         keyboard_state[KEY_F6] = 1U; break;
                case VK_F7:         keyboard_state[KEY_F7] = 1U; break;
                case VK_F8:         keyboard_state[KEY_F8] = 1U; break;
                case VK_F9:         keyboard_state[KEY_F9] = 1U; break;
                case VK_F10:        keyboard_state[KEY_F10] = 1U; break;
                case VK_F11:        keyboard_state[KEY_F11] = 1U; break;
                case VK_F12:        keyboard_state[KEY_F12] = 1U; break;
                case VK_F13:        keyboard_state[KEY_F13] = 1U; break;
                case VK_F14:        keyboard_state[KEY_F14] = 1U; break;
                case VK_F15:        keyboard_state[KEY_F15] = 1U; break;
                case VK_F16:        keyboard_state[KEY_F16] = 1U; break;
                case VK_F17:        keyboard_state[KEY_F17] = 1U; break;
                case VK_F18:        keyboard_state[KEY_F18] = 1U; break;
                case VK_F19:        keyboard_state[KEY_F19] = 1U; break;
                case VK_F20:        keyboard_state[KEY_F20] = 1U; break;
                case VK_F21:        keyboard_state[KEY_F21] = 1U; break;
                case VK_F22:        keyboard_state[KEY_F22] = 1U; break;
                case VK_F23:        keyboard_state[KEY_F23] = 1U; break;
                case VK_F24:        keyboard_state[KEY_F24] = 1U; break;
                case VK_NUMLOCK:    keyboard_state[KEY_NUMLOCK] = 1U; break;
                case VK_SCROLL:     keyboard_state[KEY_SCROLL] = 1U; break;
                case VK_LSHIFT:     keyboard_state[KEY_LSHIFT] = 1U; break;
                case VK_RSHIFT:     keyboard_state[KEY_RSHIFT] = 1U; break;
                case VK_LCONTROL:   keyboard_state[KEY_LCONTROL] = 1U; break;
                case VK_RCONTROL:   keyboard_state[KEY_RCONTROL] = 1U; break;
                case VK_LMENU:      keyboard_state[KEY_LALT] = 1U; break;
                case VK_RMENU:      keyboard_state[KEY_RALT] = 1U; break;
            }
            break;
        case WM_KEYUP:
            switch (w_param)
            {
                case VK_BACK:       keyboard_state[KEY_BACK] = 0U; break;
                case VK_TAB:        keyboard_state[KEY_TAB] = 0U; break;
                case VK_CLEAR:      keyboard_state[KEY_CLEAR] = 0U; break;
                case VK_RETURN:     keyboard_state[KEY_RETURN] = 0U; break;
                case VK_PAUSE:      keyboard_state[KEY_PAUSE] = 0U; break;
                case VK_CAPITAL:    keyboard_state[KEY_CAPS_LOCK] = 0U; break;
                case VK_ESCAPE:     keyboard_state[KEY_ESCAPE] = 0U; break;
                case VK_LEFT:       keyboard_state[KEY_LEFT] = 0U; gamepad_state.left_stick_x = 0.0f; break;
                case VK_RIGHT:      keyboard_state[KEY_RIGHT] = 0U; gamepad_state.left_stick_x = 0.0f; break;
                case VK_UP:         keyboard_state[KEY_UP] = 0U; gamepad_state.left_stick_y = 0.0f; break;
                case VK_DOWN:       keyboard_state[KEY_DOWN] = 0U; gamepad_state.left_stick_y = 0.0f; break;
                case VK_SPACE:      keyboard_state[KEY_SPACE] = 0U; break;
                case VK_PRIOR:      keyboard_state[KEY_PAGE_UP] = 0U; break;
                case VK_NEXT:       keyboard_state[KEY_PAGE_DOWN] = 0U; break;
                case VK_END:        keyboard_state[KEY_END] = 0U; break;
                case VK_PRINT:      keyboard_state[KEY_PRINT] = 0U; break;
                case VK_INSERT:     keyboard_state[KEY_INSERT] = 0U; break;
                case VK_DELETE:     keyboard_state[KEY_DELETE] = 0U; break;
                // same as ASCII '0' - '9' (0x30 - 0x39)
                case 0x30:          keyboard_state[KEY_0] = 0U; break;
                case 0x31:          keyboard_state[KEY_1] = 0U; break;
                case 0x32:          keyboard_state[KEY_2] = 0U; break;
                case 0x33:          keyboard_state[KEY_3] = 0U; break;
                case 0x34:          keyboard_state[KEY_4] = 0U; break;
                case 0x35:          keyboard_state[KEY_5] = 0U; break;
                case 0x36:          keyboard_state[KEY_6] = 0U; break;
                case 0x37:          keyboard_state[KEY_7] = 0U; break;
                case 0x38:          keyboard_state[KEY_8] = 0U; break;
                case 0x39:          keyboard_state[KEY_9] = 0U; break;
                // same as ASCII 'A' - 'Z' (0x41 - 0x5A)
                case 0x41:          keyboard_state[KEY_A] = 0U; break;
                case 0x42:          keyboard_state[KEY_B] = 0U; break;
                case 0x43:          keyboard_state[KEY_C] = 0U; break;
                case 0x44:          keyboard_state[KEY_D] = 0U; break;
                case 0x45:          keyboard_state[KEY_E] = 0U; break;
                case 0x46:          keyboard_state[KEY_F] = 0U; break;
                case 0x47:          keyboard_state[KEY_G] = 0U; break;
                case 0x48:          keyboard_state[KEY_H] = 0U; break;
                case 0x49:          keyboard_state[KEY_I] = 0U; break;
                case 0x4a:          keyboard_state[KEY_J] = 0U; break;
                case 0x4b:          keyboard_state[KEY_K] = 0U; break;
                case 0x4c:          keyboard_state[KEY_L] = 0U; break;
                case 0x4d:          keyboard_state[KEY_M] = 0U; break;
                case 0x4e:          keyboard_state[KEY_N] = 0U; break;
                case 0x4f:          keyboard_state[KEY_O] = 0U; break;
                case 0x50:          keyboard_state[KEY_P] = 0U; break;
                case 0x51:          keyboard_state[KEY_Q] = 0U; break;
                case 0x52:          keyboard_state[KEY_R] = 0U; break;
                case 0x53:          keyboard_state[KEY_S] = 0U; break;
                case 0x54:          keyboard_state[KEY_T] = 0U; break;
                case 0x55:          keyboard_state[KEY_U] = 0U; break;
                case 0x56:          keyboard_state[KEY_V] = 0U; break;
                case 0x57:          keyboard_state[KEY_W] = 0U; break;
                case 0x58:          keyboard_state[KEY_X] = 0U; break;
                case 0x59:          keyboard_state[KEY_Y] = 0U; break;
                case 0x5a:          keyboard_state[KEY_Z] = 0U; break;
                case VK_LWIN:       keyboard_state[KEY_LWIN] = 0U; break;
                case VK_RWIN:       keyboard_state[KEY_RWIN] = 0U; break;
                case VK_APPS:       keyboard_state[KEY_APPS] = 0U; break;
                case VK_NUMPAD0:    keyboard_state[KEY_NUMPAD0] = 0U; break;
                case VK_NUMPAD1:    keyboard_state[KEY_NUMPAD1] = 0U; break;
                case VK_NUMPAD2:    keyboard_state[KEY_NUMPAD2] = 0U; break;
                case VK_NUMPAD3:    keyboard_state[KEY_NUMPAD3] = 0U; break;
                case VK_NUMPAD4:    keyboard_state[KEY_NUMPAD4] = 0U; break;
                case VK_NUMPAD5:    keyboard_state[KEY_NUMPAD5] = 0U; break;
                case VK_NUMPAD6:    keyboard_state[KEY_NUMPAD6] = 0U; break;
                case VK_NUMPAD7:    keyboard_state[KEY_NUMPAD7] = 0U; break;
                case VK_NUMPAD8:    keyboard_state[KEY_NUMPAD8] = 0U; break;
                case VK_NUMPAD9:    keyboard_state[KEY_NUMPAD9] = 0U; break;
                case VK_MULTIPLY:   keyboard_state[KEY_MULTIPLY] = 0U; break;
                case VK_ADD:        keyboard_state[KEY_ADD] = 0U; break;
                case VK_SEPARATOR:  keyboard_state[KEY_SEPARATOR] = 0U; break;
                case VK_SUBTRACT:   keyboard_state[KEY_SUBTRACT] = 0U; break;
                case VK_DECIMAL:    keyboard_state[KEY_DECIMAL] = 0U; break;
                case VK_DIVIDE:     keyboard_state[KEY_DIVIDE] = 0U; break;
                case VK_F1:         keyboard_state[KEY_F1] = 0U; break;
                case VK_F2:         keyboard_state[KEY_F2] = 0U; break;
                case VK_F3:         keyboard_state[KEY_F3] = 0U; break;
                case VK_F4:         keyboard_state[KEY_F4] = 0U; break;
                case VK_F5:         keyboard_state[KEY_F5] = 0U; break;
                case VK_F6:         keyboard_state[KEY_F6] = 0U; break;
                case VK_F7:         keyboard_state[KEY_F7] = 0U; break;
                case VK_F8:         keyboard_state[KEY_F8] = 0U; break;
                case VK_F9:         keyboard_state[KEY_F9] = 0U; break;
                case VK_F10:        keyboard_state[KEY_F10] = 0U; break;
                case VK_F11:        keyboard_state[KEY_F11] = 0U; break;
                case VK_F12:        keyboard_state[KEY_F12] = 0U; break;
                case VK_F13:        keyboard_state[KEY_F13] = 0U; break;
                case VK_F14:        keyboard_state[KEY_F14] = 0U; break;
                case VK_F15:        keyboard_state[KEY_F15] = 0U; break;
                case VK_F16:        keyboard_state[KEY_F16] = 0U; break;
                case VK_F17:        keyboard_state[KEY_F17] = 0U; break;
                case VK_F18:        keyboard_state[KEY_F18] = 0U; break;
                case VK_F19:        keyboard_state[KEY_F19] = 0U; break;
                case VK_F20:        keyboard_state[KEY_F20] = 0U; break;
                case VK_F21:        keyboard_state[KEY_F21] = 0U; break;
                case VK_F22:        keyboard_state[KEY_F22] = 0U; break;
                case VK_F23:        keyboard_state[KEY_F23] = 0U; break;
                case VK_F24:        keyboard_state[KEY_F24] = 0U; break;
                case VK_NUMLOCK:    keyboard_state[KEY_NUMLOCK] = 0U; break;
                case VK_SCROLL:     keyboard_state[KEY_SCROLL] = 0U; break;
                case VK_LSHIFT:     keyboard_state[KEY_LSHIFT] = 0U; break;
                case VK_RSHIFT:     keyboard_state[KEY_RSHIFT] = 0U; break;
                case VK_LCONTROL:   keyboard_state[KEY_LCONTROL] = 0U; break;
                case VK_RCONTROL:   keyboard_state[KEY_RCONTROL] = 0U; break;
                case VK_LMENU:      keyboard_state[KEY_LALT] = 0U; break;
                case VK_RMENU:      keyboard_state[KEY_RALT] = 0U; break;
            }
            break;
        case WM_MOUSEMOVE:
            current_mouse_state.x = GET_X_LPARAM(l_param);
            current_mouse_state.y = GET_Y_LPARAM(l_param);
            break;
        case WM_MOUSEWHEEL:
            current_mouse_state.wheel = GET_WHEEL_DELTA_WPARAM(w_param);
            break;
        case WM_LBUTTONDOWN:
            current_mouse_state.buttons |= MOUSE_LEFT_BUTTON;
            break;
        case WM_LBUTTONUP:
            current_mouse_state.buttons &= ~MOUSE_LEFT_BUTTON;
            break;
        case WM_MBUTTONDOWN:
            current_mouse_state.buttons |= MOUSE_MIDDLE_BUTTON;
            break;
        case WM_MBUTTONUP:
            current_mouse_state.buttons &= ~MOUSE_MIDDLE_BUTTON;
            break;
        case WM_RBUTTONDOWN:
            current_mouse_state.buttons |= MOUSE_RIGHT_BUTTON;
            break;
        case WM_RBUTTONUP:
            current_mouse_state.buttons &= ~MOUSE_RIGHT_BUTTON;
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
        DWORD result = XInputGetState(0U, &state);
        if (result == ERROR_SUCCESS)
        {
            // Buttons
            WORD buttons = state.Gamepad.wButtons;
            if (buttons & XINPUT_GAMEPAD_DPAD_UP) gps->buttons |= GAMEPAD_DPAD_UP;
            if (buttons & XINPUT_GAMEPAD_DPAD_DOWN) gps->buttons |= GAMEPAD_DPAD_DOWN;
            if (buttons & XINPUT_GAMEPAD_DPAD_LEFT) gps->buttons |= GAMEPAD_DPAD_LEFT;
            if (buttons & XINPUT_GAMEPAD_DPAD_RIGHT) gps->buttons |= GAMEPAD_DPAD_RIGHT;
            if (buttons & XINPUT_GAMEPAD_START) gps->buttons |= GAMEPAD_START;
            if (buttons & XINPUT_GAMEPAD_BACK) gps->buttons |= GAMEPAD_BACK;
            if (buttons & XINPUT_GAMEPAD_LEFT_THUMB) gps->buttons |= GAMEPAD_LEFT_THUMB;
            if (buttons & XINPUT_GAMEPAD_RIGHT_THUMB) gps->buttons |= GAMEPAD_RIGHT_THUMB;
            if (buttons & XINPUT_GAMEPAD_LEFT_SHOULDER) gps->buttons |= GAMEPAD_LEFT_SHOULDER;
            if (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER) gps->buttons |= GAMEPAD_RIGHT_SHOULDER;
            if (buttons & XINPUT_GAMEPAD_A) gps->buttons |= GAMEPAD_A;
            if (buttons & XINPUT_GAMEPAD_B) gps->buttons |= GAMEPAD_B;
            if (buttons & XINPUT_GAMEPAD_X) gps->buttons |= GAMEPAD_X;
            if (buttons & XINPUT_GAMEPAD_Y) gps->buttons |= GAMEPAD_Y;

            // Analog sticks
            ce_input_filter_dead_zone((float)state.Gamepad.sThumbLX, (float)state.Gamepad.sThumbLY, &gps->left_stick_x, &gps->left_stick_y, (float)XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, 32768.0f);
            ce_input_filter_dead_zone((float)state.Gamepad.sThumbRX, (float)state.Gamepad.sThumbRY, &gps->right_stick_x, &gps->right_stick_y, (float)XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, 32768.0f);

            // Trigger
            ce_input_filter_dead_zone(0.0f, (float)state.Gamepad.bLeftTrigger, &gps->left_trigger, &gps->left_trigger, (float)XINPUT_GAMEPAD_TRIGGER_THRESHOLD, 256.0f);
            ce_input_filter_dead_zone(0.0f, (float)state.Gamepad.bRightTrigger, &gps->right_trigger, &gps->right_trigger, (float)XINPUT_GAMEPAD_TRIGGER_THRESHOLD, 256.0f);
        }
        else
        {
            gamepad_connected = false;
        }
    }
    else
    {
        // wip: fallback to keyboard input
        gps->left_stick_x = gamepad_state.left_stick_x;
        gps->left_stick_y = gamepad_state.left_stick_y;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ce_input_mouse_get_state(ce_mouse_state* const ms)
{
    ms->buttons = current_mouse_state.buttons;
    ms->x = current_mouse_state.x;
    ms->y = current_mouse_state.y;
    ms->dx = current_mouse_state.x - last_mouse_state.x;
    ms->dy = current_mouse_state.y - last_mouse_state.y;
    ms->wheel = current_mouse_state.wheel;
    current_mouse_state.wheel = 0;
    last_mouse_state = current_mouse_state;
}

//------------------------------------------------------------------------------
/**
*/
void
ce_input_keyboard_get_state(ce_keyboard_state* const kbs)
{
    memcpy(kbs->keys, keyboard_state, sizeof(keyboard_state));
}
