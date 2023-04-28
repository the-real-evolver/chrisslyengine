//------------------------------------------------------------------------------
//  filters.cpp
//  (C) 2021 Christian Bleicher
//------------------------------------------------------------------------------
#include "filters.h"
#include "chrisslyplatform.h"
#include "debug.h"

//------------------------------------------------------------------------------
/**
*/
static void
lowpass_s16_mono(const short* _ChrisslyRestrict src_buffer, short* _ChrisslyRestrict dst_buffer, unsigned int num_samples, float cutoff, float store[4U])
{
    unsigned int i;
    for (i = 0U; i < num_samples; ++i)
    {
        store[0U] += cutoff * ((float)*src_buffer++ - store[0U]);
        store[1U] += cutoff * (store[0U] - store[1U]);
        *dst_buffer++ = (short)store[1U];
    }
}

//------------------------------------------------------------------------------
/**
*/
static void
lowpass_s16_stereo(const short* _ChrisslyRestrict src_buffer, short* _ChrisslyRestrict dst_buffer, unsigned int num_samples, float cutoff, float store[4U])
{
    unsigned int i;
    for (i = 0U; i < num_samples; ++i)
    {
        store[0U] += cutoff * ((float)*src_buffer++ - store[0U]);
        store[1U] += cutoff * (store[0U] - store[1U]);
        *dst_buffer++ = (short)store[1U];
        store[2U] += cutoff * ((float)*src_buffer++ - store[2U]);
        store[3U] += cutoff * (store[2U] - store[3U]);
        *dst_buffer++ = (short)store[3U];
    }
}

//------------------------------------------------------------------------------
/**
    See: https://www.martin-finke.de/articles/audio-plugins-013-filter/
*/
void
ce_audio_filter_lowpass(int bits, int num_channels, const void* const src_buffer, void* const dst_buffer, unsigned int num_samples, float cutoff, float store[4U])
{
    switch (bits)
    {
        case 16:
            {
                switch (num_channels)
                {
                    case 1:
                        lowpass_s16_mono((const short*)src_buffer, (short*)dst_buffer, num_samples, cutoff, store);
                        break;
                    case 2:
                       lowpass_s16_stereo((const short*)src_buffer, (short*)dst_buffer, num_samples, cutoff, store);
                        break;
                    default:
                        CE_ASSERT(false, "ce_audio_filter_lowpass(): filtering '%i' channels not supported\n", num_channels);
                }
            }
            break;

        default:
            CE_ASSERT(false, "ce_audio_filter_lowpass(): unsupported bitrate: '%i'\n", bits);
    }
}
