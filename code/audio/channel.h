#ifndef CHANNEL_H_
#define CHANNEL_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::Channel

    (C) 2014 Christian Bleicher
*/
#include "sound.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace audio
{

class Channel
{
public:
    /// special channel index values
    enum ChannelIndex
    {
        /// use the next free channel available
        CHANNEL_FREE  = -1,
        CHANNEL_REUSE = -2
    };

    /// default constructor
    Channel();
    /// destructor
    ~Channel();
    /// returns the currently playing sound for this channel
    bool GetCurrentSound(Sound** sound);
    /// retrieves the internal channel index for a channel
    bool GetIndex(int* index);
    /// returns the current PCM offset or playback position for the specified channel
    bool GetPosition(unsigned int* position);
    /// returns the playing state for the current channel
    bool IsPlaying(bool* isplaying);
    /// sets the current playback position for the currently playing sound to the specified PCM offset
    bool SetPosition(unsigned int  position);
    /// stops the channel from playing. Makes it available for re-use by the priority system
    bool Stop();

    /// set the sound to playback on this channel
    void _AttachSound(Sound* sound);
    /// set the internal channel index for a channel
    void _SetIndex(int index);
    /// set isplaying flag
    void _SetIsPlaying(bool isplaying);

private:
    Sound* currentSound;
    int index;
    unsigned int position;
    bool isPlaying;
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
