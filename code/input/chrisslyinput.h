#ifndef CHRISSLYINPUT_H_
#define CHRISSLYINPUT_H_
//------------------------------------------------------------------------------
/**
    @file input/chrisslyinput.h

    Mainmodule that provides a platform agnostic interface to input devices like
    gamepads, keyboards and mouses.

    (C) 2022 Christian Bleicher
*/
#include "chrisslyinputplatform.h"

/// enumeration for the gamepad buttons
enum ce_gamepad_buttons
{
    // Xbox 360 Controller
    GAMEPAD_DPAD_UP          = (1 << 0),
    GAMEPAD_DPAD_DOWN        = (1 << 1),
    GAMEPAD_DPAD_LEFT        = (1 << 2),
    GAMEPAD_DPAD_RIGHT       = (1 << 3),
    GAMEPAD_START            = (1 << 4),
    GAMEPAD_BACK             = (1 << 5),
    GAMEPAD_LEFT_THUMB       = (1 << 6),
    GAMEPAD_RIGHT_THUMB      = (1 << 7),
    GAMEPAD_LEFT_SHOULDER    = (1 << 8),
    GAMEPAD_RIGHT_SHOULDER   = (1 << 9),
    GAMEPAD_A                = (1 << 10),
    GAMEPAD_B                = (1 << 11),
    GAMEPAD_X                = (1 << 12),
    GAMEPAD_Y                = (1 << 13),
    // Common Sony Controls
    GAMEPAD_HOME             = (1 << 14),
    GAMEPAD_CROSS            = GAMEPAD_A,
    GAMEPAD_CIRCLE           = GAMEPAD_B,
    GAMEPAD_SQUARE           = GAMEPAD_X,
    GAMEPAD_TRIANGLE         = GAMEPAD_Y,
    // PSP
    GAMEPAD_NOTE             = (1 << 15),
    GAMEPAD_SCREEN           = (1 << 16),
    GAMEPAD_VOLUP            = (1 << 17),
    GAMEPAD_VOLDOWN          = (1 << 18),
    GAMEPAD_SELECT           = GAMEPAD_BACK,
    // PS4
    GAMEPAD_OPTIONS          = GAMEPAD_START,
    GAMEPAD_SHARE            = GAMEPAD_BACK
};

/// describes the current state of the gamepad
struct ce_gamepad_state
{
    unsigned int buttons;   // bit mask containing zero or more of ce_gamepad_buttons
    float left_stick_x;     // the values range from -1.0 to 1.0
    float left_stick_y;     // the values range from -1.0 to 1.0
    float right_stick_x;    // the values range from -1.0 to 1.0
    float right_stick_y;    // the values range from -1.0 to 1.0
    float left_trigger;     // the values range from 0.0 to 1.0
    float right_trigger;    // the values range from 0.0 to 1.0
};

/// enumeration for the mouse buttons
enum ce_mouse_buttons
{
    MOUSE_LEFT_BUTTON   = (1 << 0),
    MOUSE_RIGHT_BUTTON  = (1 << 1),
    MOUSE_MIDDLE_BUTTON = (1 << 2)
};

/// describes the current state of the mouse
struct ce_mouse_state
{
    unsigned int buttons;   // bit mask containing zero or more of ce_mouse_buttons
    int x;                  // horizontal mouse position in pixels, relative to the upper left corner of the window
    int y;                  // vertical mouse position in pixels, relative to the upper left corner of the window
    int dx;                 // relative horizontal mouse movement since last frame
    int dy;                 // relative vertical mouse movement since last frame
    short wheel;            // distance the wheel is rotated, a positive value indicates that the wheel was rotated forward
};

/// enumeration for the keys
enum ce_key_code
{
    KEY_BACK = 0,
    KEY_TAB,
    KEY_CLEAR,
    KEY_RETURN,
    KEY_PAUSE,
    KEY_CAPS_LOCK,
    KEY_ESCAPE,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_UP,
    KEY_DOWN,
    KEY_SPACE,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN,
    KEY_END,
    KEY_PRINT,
    KEY_INSERT,
    KEY_DELETE,
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,
    KEY_LWIN,
    KEY_RWIN,
    KEY_APPS,
    KEY_NUMPAD0,
    KEY_NUMPAD1,
    KEY_NUMPAD2,
    KEY_NUMPAD3,
    KEY_NUMPAD4,
    KEY_NUMPAD5,
    KEY_NUMPAD6,
    KEY_NUMPAD7,
    KEY_NUMPAD8,
    KEY_NUMPAD9,
    KEY_MULTIPLY,
    KEY_ADD,
    KEY_SEPARATOR,
    KEY_SUBTRACT,
    KEY_DECIMAL,
    KEY_DIVIDE,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_F13,
    KEY_F14,
    KEY_F15,
    KEY_F16,
    KEY_F17,
    KEY_F18,
    KEY_F19,
    KEY_F20,
    KEY_F21,
    KEY_F22,
    KEY_F23,
    KEY_F24,
    KEY_NUMLOCK,
    KEY_SCROLL,
    KEY_LSHIFT,
    KEY_RSHIFT,
    KEY_LCONTROL,
    KEY_RCONTROL,
    KEY_LALT,
    KEY_RALT
};

/// describes the current state of the keyboard
struct ce_keyboard_state
{
    unsigned char keys[256U];
};

/// initialises the input system
void ce_input_initialise();
/// shutdown the input system
void ce_input_shutdown();
/// is there any gamepad connected?
bool ce_input_gamepad_connected();
/// retrieves the current state of the gamepad
void ce_input_gamepad_get_state(ce_gamepad_state* const gps);
/// retrieves the current state of the mouse
void ce_input_mouse_get_state(ce_mouse_state* const ms);
/// retrieves the current state of the keyboard
void ce_input_keyboard_get_state(ce_keyboard_state* const kbs);
#endif