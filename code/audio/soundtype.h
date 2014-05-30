#ifndef SOUNDTYPE_H_
#define SOUNDTYPE_H_
//------------------------------------------------------------------------------
/**
    Soundtype enumeration

    (C) 2014 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly
{
namespace audio
{
    enum SoundType
    {
        /// 3rd party / unknown plugin format
        SOUND_TYPE_UNKNOWN,
        /// MP2/MP3 MPEG
        SOUND_TYPE_MPEG,
        /// Ogg Vorbis
        SOUND_TYPE_OGGVORBIS,
        /// Microsoft RIFF WAV
        SOUND_TYPE_WAV
    };

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif