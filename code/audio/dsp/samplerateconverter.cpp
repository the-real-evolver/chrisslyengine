//------------------------------------------------------------------------------
//  samplerateconverter.cpp
//  (C) 2017 Christian Bleicher
//------------------------------------------------------------------------------
#include "samplerateconverter.h"
#include "chrisslymath.h"

//------------------------------------------------------------------------------
/**
*/
void
ce_audio_resample_s16_stereo(const int* const _ChrisslyRestrict src_buffer, unsigned int num_samples, int* const _ChrisslyRestrict dst_buffer, unsigned int num_resampled)
{
    int x0 = 0;
    float sample = 0.0f, t = 0.0f, ratio = (float)num_samples / (float)num_resampled;
    short *src = NULL, *dst = NULL;
    unsigned int i;
    for (i = 0U; i < num_resampled; ++i)
    {
        x0 = (int)sample < (int)num_samples - 1 ? (int)sample : (int)num_samples - 2;
        t = sample - x0;
        src = (short*)&src_buffer[x0];
        dst = (short*)&dst_buffer[i];
        dst[0U] = (short)ce_math_lerp((float)src[0U], (float)src[2U], t);
        dst[1U] = (short)ce_math_lerp((float)src[1U], (float)src[3U], t);
        sample += ratio;
    }
}