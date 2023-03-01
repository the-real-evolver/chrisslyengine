//------------------------------------------------------------------------------
//  mixer.cpp
//  (C) 2017 Christian Bleicher
//------------------------------------------------------------------------------
#include "mixer.h"
#include "chrisslyplatform.h"
#include "chrisslymath.h"
#include "debug.h"

#define CLAMP_SAMPLE(sample) {if ((sample) > 32767) {(sample) = 32767;} else if ((sample) < -32768) {(sample) = -32768;}}

//------------------------------------------------------------------------------
/**
*/
static void
mix_s16_stereo_s16(int num_channels, const short* _ChrisslyRestrict buffer_to_mix, short* _ChrisslyRestrict buffer, unsigned int num_samples, float volume_left, float volume_right)
{
    int sample;

    unsigned int i;
    for (i = 0U; i < num_samples; ++i)
    {
        sample = *buffer + (short)((float)(*buffer_to_mix) * volume_left);
        CLAMP_SAMPLE(sample)
        *buffer++ = (short)sample;
        if (2 == num_channels)
        {
            ++buffer_to_mix;
        }

        sample = *buffer + (short)((float)(*buffer_to_mix) * volume_right);
        CLAMP_SAMPLE(sample)
        *buffer++ = (short)sample;
        ++buffer_to_mix;
    }
}

//------------------------------------------------------------------------------
/**
*/
static void
mix_s16_stereo_u8(int num_channels, const unsigned char* _ChrisslyRestrict buffer_to_mix, short* _ChrisslyRestrict buffer, unsigned int num_samples, float volume_left, float volume_right)
{
    int sample;

    unsigned int i;
    for (i = 0U; i < num_samples; ++i)
    {
        sample = *buffer + (short)(((float)(*buffer_to_mix) - 127.0f) * 256.0f * volume_left);
        CLAMP_SAMPLE(sample)
        *buffer++ = (short)sample;
        if (2 == num_channels)
        {
            ++buffer_to_mix;
        }

        sample = *buffer + (short)(((float)(*buffer_to_mix) - 127.0f) * 256.0f * volume_right);
        CLAMP_SAMPLE(sample)
        *buffer++ = (short)sample;
        ++buffer_to_mix;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ce_audio_calculate_stereo_channel_volumes(ce_audio_panning_mode mode, float volume, float pan, float* const volume_left, float* const volume_right)
{
    switch (mode)
    {
        case PAN_STEREO:
            {
                float leftGain = 1.0f - pan;
                float rightGain = pan + 1.0f;
                *volume_left = volume * (leftGain > 1.0f ? 1.0f : leftGain);
                *volume_right = volume * (rightGain > 1.0f ? 1.0f : rightGain);
            }
            break;

        case PAN_CONSTANTPOWER:
            {
                float var = 0.785398f * (pan + 1.0f);
                *volume_left = volume * chrissly::core::Math::Cos(var);
                *volume_right = volume * chrissly::core::Math::Sin(var);
            }
            break;

        default:
            CE_ASSERT(false, "ce_audio_calculate_stereo_channel_volumes(): panningmode '%i' not supported\n", mode);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ce_audio_mix_s16_stereo(int bits, int num_channels, const void* const buffer_to_mix, short* const buffer, unsigned int num_samples, float volume, float pan)
{
    float volume_left, volume_right;
    ce_audio_calculate_stereo_channel_volumes(2 == num_channels ? PAN_STEREO : PAN_CONSTANTPOWER, volume, pan, &volume_left, &volume_right);

    switch (bits)
    {
        case 8:
            {
                switch (num_channels)
                {
                    case 1:
                    case 2:
                        mix_s16_stereo_u8(num_channels, (const unsigned char*)buffer_to_mix, buffer, num_samples, volume_left, volume_right);
                        break;
                    default:
                        CE_ASSERT(false, "ce_audio_mix_s16_stereo(): mixing '%i' channels not supported\n", num_channels);
                }
            }
            break;

        case 16:
            {
                switch (num_channels)
                {
                    case 1:
                    case 2:
                        mix_s16_stereo_s16(num_channels, (const short*)buffer_to_mix, buffer, num_samples, volume_left, volume_right);
                        break;
                    default:
                        CE_ASSERT(false, "ce_audio_mix_s16_stereo(): mixing '%i' channels not supported\n", num_channels);
                }
            }
            break;

        default:
            CE_ASSERT(false, "ce_audio_mix_s16_stereo(): unsupported bitrate: '%i'\n", bits);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ce_audio_downmix_s16_stereo_s32_mono(const short* src_buffer, int* dst_buffer, unsigned int num_samples)
{
    unsigned int i;
    for (i = 0U; i < num_samples; ++i)
    {
        short l = *src_buffer++, r = *src_buffer++;
        *dst_buffer++ = (l + r) * 32768;
    }
}
