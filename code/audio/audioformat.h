#ifndef AUDIOFORMAT_H_
#define AUDIOFORMAT_H_
//------------------------------------------------------------------------------
/**
    Audioformat enumeration

    (C) 2014 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly
{
namespace audio
{
    enum AudioFormat
    {
        /// Unknown audioformat
        AUDIO_FORMAT_NONE,
        /// 8-bit integer PCM data
        AUDIO_FORMAT_PCM8,
        /// 16-bit integer PCM data
        AUDIO_FORMAT_PCM16,
        /// Compressed MPEG layer 2 or 3 data
        AUDIO_FORMAT_MPEG
    };

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif