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
ce_resample_s16_stereo(short* _ChrisslyRestrict src_buffer, unsigned int num_samples, short* _ChrisslyRestrict dst_buffer, unsigned int num_resampled)
{
    float ratio = (float)num_samples / (float)num_resampled;
    unsigned int i;
    for (i = 0U; i < num_resampled; ++i)
    {
        unsigned int sample = (unsigned int)chrissly::core::Math::Ceil((float)i * ratio + 0.5f) - 1U;
        dst_buffer[i * 2U] = src_buffer[sample * 2U];
        dst_buffer[i * 2U + 1U] = src_buffer[sample * 2U + 1U];
    }
}