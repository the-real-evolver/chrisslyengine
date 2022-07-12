#ifndef DAMPING_H_
#define DAMPING_H_
//------------------------------------------------------------------------------
/**
    @file damping.h

    (C) 2022 Christian Bleicher
*/

/// gradually changes a value towards a desired goal over time
float ce_smooth_damp(float current, float target, float* const current_velocity, float smooth_time, float max_speed, float delta_time);

#endif