//------------------------------------------------------------------------------
//  windowsinput.cpp
//  (C) 2022 Christian Bleicher
//------------------------------------------------------------------------------
#include "input/chrisslyinput.h"
#include "input/common/commoninput.h"
#include "miscutils.h"
#include "debug.h"
#include <xinput.h>
#include <Dbt.h>
#include <windowsx.h>

static bool gamepad_check_availability = true;
static bool gamepad_connected = false;

static ce_mouse_state current_mouse_state = {0U, 0, 0, 0, 0};
static ce_mouse_state last_mouse_state = {0U, 0, 0, 0, 0};

static unsigned char keyboard_state[256U] = {0U};

static HWND window_handle = NULL;

//------------------------------------------------------------------------------
/**
*/
static void
ce_input_update_keyboard_state(WPARAM w_param, unsigned char state)
{
    switch (w_param)
    {
        case VK_BACK:       keyboard_state[KEY_BACK] = state; break;
        case VK_TAB:        keyboard_state[KEY_TAB] = state; break;
        case VK_CLEAR:      keyboard_state[KEY_CLEAR] = state; break;
        case VK_RETURN:     keyboard_state[KEY_RETURN] = state; break;
        case VK_SHIFT:
            keyboard_state[KEY_LSHIFT] = (GetAsyncKeyState(VK_LSHIFT) & 0x8000) != 0;
            keyboard_state[KEY_RSHIFT] = (GetAsyncKeyState(VK_RSHIFT) & 0x8000) != 0;
            break;
        case VK_CONTROL:
            keyboard_state[KEY_LCONTROL] = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
            keyboard_state[KEY_RCONTROL] = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
            break;
        case VK_MENU:
            keyboard_state[KEY_LALT] = (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0;
            keyboard_state[KEY_RALT] = (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0;
            break;
        case VK_PAUSE:      keyboard_state[KEY_PAUSE] = state; break;
        case VK_CAPITAL:    keyboard_state[KEY_CAPS_LOCK] = state; break;
        case VK_ESCAPE:     keyboard_state[KEY_ESCAPE] = state; break;
        case VK_LEFT:       keyboard_state[KEY_LEFT] = state; break;
        case VK_RIGHT:      keyboard_state[KEY_RIGHT] = state; break;
        case VK_UP:         keyboard_state[KEY_UP] = state; break;
        case VK_DOWN:       keyboard_state[KEY_DOWN] = state; break;
        case VK_SPACE:      keyboard_state[KEY_SPACE] = state; break;
        case VK_PRIOR:      keyboard_state[KEY_PAGE_UP] = state; break;
        case VK_NEXT:       keyboard_state[KEY_PAGE_DOWN] = state; break;
        case VK_END:        keyboard_state[KEY_END] = state; break;
        case VK_PRINT:      keyboard_state[KEY_PRINT] = state; break;
        case VK_INSERT:     keyboard_state[KEY_INSERT] = state; break;
        case VK_DELETE:     keyboard_state[KEY_DELETE] = state; break;
        // same as ASCII '0' - '9' (0x30 - 0x39)
        case 0x30:          keyboard_state[KEY_0] = state; break;
        case 0x31:          keyboard_state[KEY_1] = state; break;
        case 0x32:          keyboard_state[KEY_2] = state; break;
        case 0x33:          keyboard_state[KEY_3] = state; break;
        case 0x34:          keyboard_state[KEY_4] = state; break;
        case 0x35:          keyboard_state[KEY_5] = state; break;
        case 0x36:          keyboard_state[KEY_6] = state; break;
        case 0x37:          keyboard_state[KEY_7] = state; break;
        case 0x38:          keyboard_state[KEY_8] = state; break;
        case 0x39:          keyboard_state[KEY_9] = state; break;
        // same as ASCII 'A' - 'Z' (0x41 - 0x5A)
        case 0x41:          keyboard_state[KEY_A] = state; break;
        case 0x42:          keyboard_state[KEY_B] = state; break;
        case 0x43:          keyboard_state[KEY_C] = state; break;
        case 0x44:          keyboard_state[KEY_D] = state; break;
        case 0x45:          keyboard_state[KEY_E] = state; break;
        case 0x46:          keyboard_state[KEY_F] = state; break;
        case 0x47:          keyboard_state[KEY_G] = state; break;
        case 0x48:          keyboard_state[KEY_H] = state; break;
        case 0x49:          keyboard_state[KEY_I] = state; break;
        case 0x4a:          keyboard_state[KEY_J] = state; break;
        case 0x4b:          keyboard_state[KEY_K] = state; break;
        case 0x4c:          keyboard_state[KEY_L] = state; break;
        case 0x4d:          keyboard_state[KEY_M] = state; break;
        case 0x4e:          keyboard_state[KEY_N] = state; break;
        case 0x4f:          keyboard_state[KEY_O] = state; break;
        case 0x50:          keyboard_state[KEY_P] = state; break;
        case 0x51:          keyboard_state[KEY_Q] = state; break;
        case 0x52:          keyboard_state[KEY_R] = state; break;
        case 0x53:          keyboard_state[KEY_S] = state; break;
        case 0x54:          keyboard_state[KEY_T] = state; break;
        case 0x55:          keyboard_state[KEY_U] = state; break;
        case 0x56:          keyboard_state[KEY_V] = state; break;
        case 0x57:          keyboard_state[KEY_W] = state; break;
        case 0x58:          keyboard_state[KEY_X] = state; break;
        case 0x59:          keyboard_state[KEY_Y] = state; break;
        case 0x5a:          keyboard_state[KEY_Z] = state; break;
        case VK_LWIN:       keyboard_state[KEY_LWIN] = state; break;
        case VK_RWIN:       keyboard_state[KEY_RWIN] = state; break;
        case VK_APPS:       keyboard_state[KEY_APPS] = state; break;
        case VK_NUMPAD0:    keyboard_state[KEY_NUMPAD0] = state; break;
        case VK_NUMPAD1:    keyboard_state[KEY_NUMPAD1] = state; break;
        case VK_NUMPAD2:    keyboard_state[KEY_NUMPAD2] = state; break;
        case VK_NUMPAD3:    keyboard_state[KEY_NUMPAD3] = state; break;
        case VK_NUMPAD4:    keyboard_state[KEY_NUMPAD4] = state; break;
        case VK_NUMPAD5:    keyboard_state[KEY_NUMPAD5] = state; break;
        case VK_NUMPAD6:    keyboard_state[KEY_NUMPAD6] = state; break;
        case VK_NUMPAD7:    keyboard_state[KEY_NUMPAD7] = state; break;
        case VK_NUMPAD8:    keyboard_state[KEY_NUMPAD8] = state; break;
        case VK_NUMPAD9:    keyboard_state[KEY_NUMPAD9] = state; break;
        case VK_MULTIPLY:   keyboard_state[KEY_MULTIPLY] = state; break;
        case VK_ADD:        keyboard_state[KEY_ADD] = state; break;
        case VK_SEPARATOR:  keyboard_state[KEY_SEPARATOR] = state; break;
        case VK_SUBTRACT:   keyboard_state[KEY_SUBTRACT] = state; break;
        case VK_DECIMAL:    keyboard_state[KEY_DECIMAL] = state; break;
        case VK_DIVIDE:     keyboard_state[KEY_DIVIDE] = state; break;
        case VK_F1:         keyboard_state[KEY_F1] = state; break;
        case VK_F2:         keyboard_state[KEY_F2] = state; break;
        case VK_F3:         keyboard_state[KEY_F3] = state; break;
        case VK_F4:         keyboard_state[KEY_F4] = state; break;
        case VK_F5:         keyboard_state[KEY_F5] = state; break;
        case VK_F6:         keyboard_state[KEY_F6] = state; break;
        case VK_F7:         keyboard_state[KEY_F7] = state; break;
        case VK_F8:         keyboard_state[KEY_F8] = state; break;
        case VK_F9:         keyboard_state[KEY_F9] = state; break;
        case VK_F10:        keyboard_state[KEY_F10] = state; break;
        case VK_F11:        keyboard_state[KEY_F11] = state; break;
        case VK_F12:        keyboard_state[KEY_F12] = state; break;
        case VK_F13:        keyboard_state[KEY_F13] = state; break;
        case VK_F14:        keyboard_state[KEY_F14] = state; break;
        case VK_F15:        keyboard_state[KEY_F15] = state; break;
        case VK_F16:        keyboard_state[KEY_F16] = state; break;
        case VK_F17:        keyboard_state[KEY_F17] = state; break;
        case VK_F18:        keyboard_state[KEY_F18] = state; break;
        case VK_F19:        keyboard_state[KEY_F19] = state; break;
        case VK_F20:        keyboard_state[KEY_F20] = state; break;
        case VK_F21:        keyboard_state[KEY_F21] = state; break;
        case VK_F22:        keyboard_state[KEY_F22] = state; break;
        case VK_F23:        keyboard_state[KEY_F23] = state; break;
        case VK_F24:        keyboard_state[KEY_F24] = state; break;
        case VK_NUMLOCK:    keyboard_state[KEY_NUMLOCK] = state; break;
        case VK_SCROLL:     keyboard_state[KEY_SCROLL] = state; break;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ce_input_wnd_proc_handler(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    CE_UNREFERENCED_PARAMETER(l_param);

    window_handle = hwnd;

    switch (msg)
    {
        case WM_DEVICECHANGE:
            if ((UINT)w_param == DBT_DEVNODES_CHANGED) gamepad_check_availability = true;
            break;
        case WM_KEYDOWN:
            ce_input_update_keyboard_state(w_param, 1U);
            break;
        case WM_KEYUP:
            ce_input_update_keyboard_state(w_param, 0U);
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
    ZeroMemory(gps, sizeof(*gps));

    if (gamepad_check_availability)
    {
        XINPUT_CAPABILITIES caps = {};
        gamepad_connected = XInputGetCapabilities(0U, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS;
        gamepad_check_availability = false;
    }

    if (gamepad_connected)
    {
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
        // fallback to keyboard input
        // Buttons
        if (1U == keyboard_state[KEY_F]) gps->buttons |= GAMEPAD_DPAD_UP;
        if (1U == keyboard_state[KEY_V]) gps->buttons |= GAMEPAD_DPAD_DOWN;
        if (1U == keyboard_state[KEY_C]) gps->buttons |= GAMEPAD_DPAD_LEFT;
        if (1U == keyboard_state[KEY_B]) gps->buttons |= GAMEPAD_DPAD_RIGHT;
        if (1U == keyboard_state[KEY_F2]) gps->buttons |= GAMEPAD_START;
        if (1U == keyboard_state[KEY_F1]) gps->buttons |= GAMEPAD_BACK;
        if (1U == keyboard_state[KEY_LSHIFT]) gps->buttons |= GAMEPAD_LEFT_THUMB;
        if (1U == keyboard_state[KEY_RSHIFT]) gps->buttons |= GAMEPAD_RIGHT_THUMB;
        if (1U == keyboard_state[KEY_LCONTROL]) gps->buttons |= GAMEPAD_LEFT_SHOULDER;
        if (1U == keyboard_state[KEY_RCONTROL]) gps->buttons |= GAMEPAD_RIGHT_SHOULDER;
        if (1U == keyboard_state[KEY_K]) gps->buttons |= GAMEPAD_A;
        if (1U == keyboard_state[KEY_L]) gps->buttons |= GAMEPAD_B;
        if (1U == keyboard_state[KEY_J]) gps->buttons |= GAMEPAD_X;
        if (1U == keyboard_state[KEY_I]) gps->buttons |= GAMEPAD_Y;

        // Analog sticks
        if (1U == keyboard_state[KEY_A])
        {
            gps->left_stick_x = -1.0f;
        }
        else if (1U == keyboard_state[KEY_D])
        {
            gps->left_stick_x = 1.0f;
        }
        if (1U == keyboard_state[KEY_W])
        {
            gps->left_stick_y = 1.0f;
        }
        else if (1U == keyboard_state[KEY_S])
        {
            gps->left_stick_y = -1.0f;
        }
        if (1U == keyboard_state[KEY_A] && 1U == keyboard_state[KEY_W])
        {
            gps->left_stick_x = -0.7f;
            gps->left_stick_y = 0.7f;
        }
        else if (1U == keyboard_state[KEY_D] && 1U == keyboard_state[KEY_W])
        {
            gps->left_stick_x = 0.7f;
            gps->left_stick_y = 0.7f;
        }
        else if (1U == keyboard_state[KEY_A] && 1U == keyboard_state[KEY_S])
        {
            gps->left_stick_x = -0.7f;
            gps->left_stick_y = -0.7f;
        }
        else if (1U == keyboard_state[KEY_D] && 1U == keyboard_state[KEY_S])
        {
            gps->left_stick_x = 0.7f;
            gps->left_stick_y = -0.7f;
        }

        if (1U == keyboard_state[KEY_LEFT])
        {
            gps->right_stick_x = -1.0f;
        }
        else if (1U == keyboard_state[KEY_RIGHT])
        {
            gps->right_stick_x = 1.0f;
        }
        if (1U == keyboard_state[KEY_UP])
        {
            gps->right_stick_y = 1.0f;
        }
        else if (1U == keyboard_state[KEY_DOWN])
        {
            gps->right_stick_y = -1.0f;
        }
        if (1U == keyboard_state[KEY_LEFT] && 1U == keyboard_state[KEY_UP])
        {
            gps->right_stick_x = -0.7f;
            gps->right_stick_y = 0.7f;
        }
        else if (1U == keyboard_state[KEY_RIGHT] && 1U == keyboard_state[KEY_UP])
        {
            gps->right_stick_x = 0.7f;
            gps->right_stick_y = 0.7f;
        }
        else if (1U == keyboard_state[KEY_LEFT] && 1U == keyboard_state[KEY_DOWN])
        {
            gps->right_stick_x = -0.7f;
            gps->right_stick_y = -0.7f;
        }
        else if (1U == keyboard_state[KEY_RIGHT] && 1U == keyboard_state[KEY_DOWN])
        {
            gps->right_stick_x = 0.7f;
            gps->right_stick_y = -0.7f;
        }

        // Trigger
        if (1U == keyboard_state[KEY_M]) gps->left_trigger = 1.0f;
        if (1U == keyboard_state[KEY_DIVIDE]) gps->right_trigger = 1.0f;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ce_input_mouse_get_state(ce_mouse_state* const ms)
{
    POINT p = {};
    if (GetCursorPos(&p))
    {
        if (ScreenToClient(window_handle, &p))
        {
            current_mouse_state.x = (int)p.x;
            current_mouse_state.y = (int)p.y;
        }
    }

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
