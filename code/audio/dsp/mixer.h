#ifndef MIXER_H_
#define MIXER_H_
//------------------------------------------------------------------------------
/**
    @file audio/dsp/mixer.h

    Low level audio signal mixing functions.

    (C) 2017 Christian Bleicher
*/

/// describes the panning mode used during playback or mixing of a channel
enum ce_audio_panning_mode
{
    /// stereo having at least one channel at full volume -1.0 = full left muted right, 0.0 = full left and right, 1.0 = full right muted left
    PAN_STEREO,
    /// constant power pan = 0.0, the balance for the sound in each speaker is 71% left and 71% right
    PAN_CONSTANTPOWER
};

/// calculate volumes of stereo channels from volume, panning and pan mode
void ce_audio_calculate_stereo_channel_volumes(ce_audio_panning_mode mode, float volume, float pan, float* const volume_left, float* const volume_right);
/// mix the given buffer with a signed 16 bit stereo buffer to signed 16 bit stereo output
void ce_audio_mix_s16_stereo(int bits, int num_channels, const void* const buffer_to_mix, short* const buffer, unsigned int num_samples, float volume, float pan);

#endif