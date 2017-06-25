#ifndef MODEFLAGS_H_
#define MODEFLAGS_H_
//------------------------------------------------------------------------------
/**
    Modeflag enumeration

    (C) 2014 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly
{
namespace audio
{
    typedef unsigned int Mode;

    enum ModeFlag
    {
        /// DEFAULT is a default sound type. Equivalent to all the defaults listed below. MODE_LOOP_OFF, MODE_2D
        MODE_DEFAULT = 0x00000000,
        /// For non looping sounds. (DEFAULT). Overrides MODE_LOOP_NORMAL
        MODE_LOOP_OFF = 0x00000001,
        /// For forward looping sounds
        MODE_LOOP_NORMAL = 0x00000002,
        /// Ignores any 3d processing. (DEFAULT)
        MODE_2D = 0x00000008,
        /// Makes the sound positionable in 3D. Overrides 2D
        MODE_3D = 0x00000010,
        /// Decompress at runtime, streaming from the source provided (ie from disk). Overrides MODE_CREATESAMPLE
        MODE_CREATESTREAM = 0x00000080,
        /// Decompress at loadtime, decompressing or decoding whole file into memory as the target sample format (ie PCM)
        MODE_CREATESAMPLE = 0x00000100,
        /// Load Ogg into memory and leave it compressed. During playback the codec will decode it in realtime as a 'compressed sample'. Overrides MODE_CREATESAMPLE. If the sound data is not Ogg it will behave as if it was created with MODE_CREATESAMPLE and decode the sound into PCM
        MODE_CREATECOMPRESSEDSAMPLE = 0x00000200
    };

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif