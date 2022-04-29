#ifndef CHRISSLYINPUT_H_
#define CHRISSLYINPUT_H_
//------------------------------------------------------------------------------
/**
    @file input/chrisslyinput.h

    Mainmodule that provides a platform agnostic interface to input devices like
    gamepads.

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
    float leftStickX;       // the values range from -1.0 to 1.0
    float leftStickY;       // the values range from -1.0 to 1.0
    float rightStickX;      // the values range from -1.0 to 1.0
    float rightStickY;      // the values range from -1.0 to 1.0
    float leftTrigger;      // the values range from 0.0 to 1.0
    float rightTrigger;     // the values range from 0.0 to 1.0
};

/// initialises the input system
void ce_input_initialise();
/// shutdown the input system
void ce_input_shutdown();
/// is there any gamepad connected?
bool ce_input_gamepad_connected();
/// retrieves the current state of the gamepad
void ce_input_gamepad_get_state(ce_gamepad_state* const gps);
#endif