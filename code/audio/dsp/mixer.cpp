//------------------------------------------------------------------------------
//  mixer.cpp
//  (C) 2017 Christian Bleicher
//------------------------------------------------------------------------------
#include "mixer.h"
#include "debug.h"
#include "chrisslymath.h"

#define CLAMP_SAMPLE(sample) {if (sample > 1.0f) {sample = 1.0f;} else if (sample < -1.0f) {sample = -1.0f;}}

//------------------------------------------------------------------------------
/**
*/
static void
mix_signed16_stereo_signed16(int num_channels, short* buffer_to_mix, short* buffer, unsigned int num_samples, float volume_left, float volume_right)
{
    float sample;

    unsigned int i;
    for (i = 0; i < num_samples; ++i)
    {
        sample = ((float)(*buffer) / 32767.0f) + ((float)(*buffer_to_mix) / 32767.0f) * volume_left;
        CLAMP_SAMPLE(sample);
        *buffer = (short)(sample * 32767.0f);
        ++buffer;
        if (2 == num_channels)
        {
            ++buffer_to_mix;
        }

        sample = ((float)(*buffer) / 32767.0f) + ((float)(*buffer_to_mix) / 32767.0f) * volume_right;
        CLAMP_SAMPLE(sample);
        *buffer = (short)(sample * 32767.0f);
        ++buffer;
        ++buffer_to_mix;
    }
}

//------------------------------------------------------------------------------
/**
*/
static void
mix_signed16_stereo_unsigned8(int num_channels, unsigned char* buffer_to_mix, short* buffer, unsigned int num_samples, float volume_left, float volume_right)
{
    float sample;

    unsigned int i;
    for (i = 0; i < num_samples; ++i)
    {
        sample = ((float)(*buffer) / 32767.0f) + (((float)(*buffer_to_mix) - 127.0f) / 128.0f) * volume_left;
        CLAMP_SAMPLE(sample);
        *buffer = (short)(sample * 32767.0f);
        ++buffer;
        if (2 == num_channels)
        {
            ++buffer_to_mix;
        }

        sample = ((float)(*buffer) / 32767.0f) + (((float)(*buffer_to_mix) - 127.0f) / 128.0f) * volume_right;
        CLAMP_SAMPLE(sample);
        *buffer = (short)(sample * 32767.0f);
        ++buffer;
        ++buffer_to_mix;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ce_audio_calculate_stereo_channel_volumes(ce_audio_panning_mode mode, float volume, float pan, float* volume_left, float* volume_right)
{
    switch (mode)
    {
        case PAN_STEREO:
            {
                float leftGain = 1.0f - pan;
                float rightGain = pan + 1.0f;
                *volume_left = volume * ((leftGain > 1.0f) ? 1.0f : leftGain);
                *volume_right = volume * ((rightGain > 1.0f) ? 1.0f : rightGain);
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
ce_audio_mix_signed16_stereo(int bits, int num_channels, void* buffer_to_mix, short* buffer, unsigned int num_samples, float volume, float pan)
{
    float volume_left, volume_right;
    ce_audio_calculate_stereo_channel_volumes(PAN_CONSTANTPOWER, volume, pan, &volume_left, &volume_right);

    switch (bits)
    {
        case 8:
            {
                switch (num_channels)
                {
                    case 1:
                    case 2:
                        mix_signed16_stereo_unsigned8(num_channels, (unsigned char*)buffer_to_mix, buffer, num_samples, volume_left, volume_right);
                        break;
                    default:
                        CE_ASSERT(false, "ce_audio_mix_signed16_stereo(): mixing '%i' channels not supported\n", num_channels);
                }
            }
            break;

        case 16:
            {
                switch (num_channels)
                {
                    case 1:
                    case 2:
                        mix_signed16_stereo_signed16(num_channels, (short*)buffer_to_mix, buffer, num_samples, volume_left, volume_right);
                        break;
                    default:
                        CE_ASSERT(false, "ce_audio_mix_signed16_stereo(): mixing '%i' channels not supported\n", num_channels);
                }
            }
            break;

        default:
            CE_ASSERT(false, "ce_audio_mix_signed16_stereo(): unsupported bitrate: '%i'\n", bits);
    }
}
