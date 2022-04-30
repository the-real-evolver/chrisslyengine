#ifndef COMMONINPUT_H_
#define COMMONINPUT_H_
//------------------------------------------------------------------------------
/**
    @file input/common/commoninput.h

    Platform agnostic helper functions.

    (C) 2022 Christian Bleicher
*/

/// clip input at deadzone and scale it according to the clipped range
void ce_input_filter_dead_zone(float x, float y, float* const x_out, float* const y_out, float dead_zone, float axis_range);
#endif