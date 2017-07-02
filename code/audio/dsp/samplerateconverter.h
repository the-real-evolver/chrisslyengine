#ifndef SAMPLERATECONVERTER_H_
#define SAMPLERATECONVERTER_H_
//------------------------------------------------------------------------------
/**
    @file audio/dsp/samplerateconverter.h

    Low level audio signal samplerate conversion functions.

    (C) 2017 Christian Bleicher
*/
#include "chrisslyplatform.h"

/// resample the given signed 16 bit stereo buffer to signed 16 bit stereo output (using nearest neighbor interpolation)
void ce_resample_s16_stereo(const int* const _ChrisslyRestrict src_buffer, unsigned int num_samples, int* const _ChrisslyRestrict dst_buffer, unsigned int num_resampled);

#endif