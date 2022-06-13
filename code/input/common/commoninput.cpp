//------------------------------------------------------------------------------
//  commoninput.cpp
//  (C) 2022 Christian Bleicher
//------------------------------------------------------------------------------
#include "input/common/commoninput.h"
#include "core/chrisslymath.h"

//------------------------------------------------------------------------------
/**
    A nice explanation on the topic can be found here:
    https://www.gamedeveloper.com/disciplines/doing-thumbstick-dead-zones-right
*/
void
ce_input_filter_dead_zone(float x, float y, float* const x_out, float* const y_out, float dead_zone, float axis_range)
{
    float mag = chrissly::core::Math::Sqrt(x * x + y * y);
    if (mag > dead_zone)
    {
        float norm_mag = (mag - dead_zone) / (axis_range - dead_zone);
        x = (x / mag) * norm_mag;
        if (x < -1.0f) x = -1.0f;
        *x_out = x > 1.0f ? 1.0f : x;
        y = (y / mag) * norm_mag;
        if (y < -1.0f) y = -1.0f;
        *y_out = y > 1.0f ? 1.0f : y;
    }
    else
    {
        *x_out = 0.0f;
        *y_out = 0.0f;
    }
}
