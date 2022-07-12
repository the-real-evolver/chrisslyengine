//------------------------------------------------------------------------------
//  damping.cpp
//  (C) 2022 Christian Bleicher
//------------------------------------------------------------------------------
#include "damping.h"

//------------------------------------------------------------------------------
/**
*/
float
ce_smooth_damp(float current, float target, float* const current_velocity, float smooth_time, float max_speed, float delta_time)
{
    // based on Game Programming Gems 4 Chapter 1.10
    smooth_time = 0.0001f > smooth_time ? 0.0001f : smooth_time;
    float omega = 2.0f / smooth_time;

    float x = omega * delta_time;
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
    float change = current - target;
    float original_to = target;

    // clamp maximum speed
    float max_change = max_speed * smooth_time;
    if (change < -max_change) change = -max_change;
    if (change > max_change) change = max_change;
    target = current - change;

    float temp = (*current_velocity + omega * change) * delta_time;
    *current_velocity = (*current_velocity - omega * temp) * exp;
    float output = target + (change + temp) * exp;

    // prevent overshooting
    if ((original_to - current > 0.0f) == (output > original_to))
    {
        output = original_to;
        *current_velocity = (output - original_to) / delta_time;
    }

    return output;
}
