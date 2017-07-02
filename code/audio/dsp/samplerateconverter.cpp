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
ce_resample_s16_stereo(const int* const _ChrisslyRestrict src_buffer, unsigned int num_samples, int* const _ChrisslyRestrict dst_buffer, unsigned int num_resampled)
{
    float ratio = (float)num_samples / (float)num_resampled;
    unsigned int i;
    for (i = 0U; i < num_resampled; ++i)
    {
        dst_buffer[i] = src_buffer[(unsigned int)chrissly::core::Math::Ceil((float)i * ratio + 0.5f) - 1U];
    }
}