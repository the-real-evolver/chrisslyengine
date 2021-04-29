#ifndef FILTERS_H_
#define FILTERS_H_
//------------------------------------------------------------------------------
/**
    @file audio/dsp/filters.h

    Low level audio filter functions.

    (C) 2021 Christian Bleicher
*/

/// apply lowpass filter to the given buffer
void ce_audio_filter_lowpass(int bits, int num_channels, const void* const src_buffer, void* const dst_buffer, unsigned int num_samples, float cutoff, float store[4U]);

#endif