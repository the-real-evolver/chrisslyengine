#ifndef CHANNELBASE_H_
#define CHANNELBASE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::ChannelBase

    (C) 2014 Christian Bleicher
*/
#include "sound.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace audio
{

class ChannelBase
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
    ChannelBase();
    /// destructor
    virtual ~ChannelBase();
    /// returns the currently playing sound for this channel
    Result GetCurrentSound(Sound** sound);
    /// retrieves the internal channel index for a channel
    Result GetIndex(int* index);
    /// returns the current PCM offset or playback position for the specified channel
    Result GetPosition(unsigned int* position);
    /// returns the playing state for the current channel
    Result IsPlaying(bool* isplaying);
    /// sets the current playback position for the currently playing sound to the specified PCM offset
    Result SetPosition(unsigned int  position);
    /// stops the channel from playing. Makes it available for re-use by the priority system
    Result Stop();
    /// sets the volume for the channel linearly (reaches from 0.0 to 1.0 inclusive. 0.0 = silent, 1.0 = full volume. Default = 1.0)
    Result SetVolume(float volume);
    /// retrieves the volume level for the channel
    Result GetVolume(float* volume);
    /// sets a channels pan position linearly (a left/right pan level, from -1.0 to 1.0 inclusive. -1.0 = Full left, 0.0 = center, 1.0 = full right. Default = 0.0)
    Result SetPan(float pan);
    /// Returns the pan position of the channel
    Result GetPan(float* pan);
    /// changes some attributes for a channel based on the mode passed in
    Result SetMode(Mode mode);
    /// retrieves the current mode bit flags for the current channel
    Result GetMode(Mode* mode);

    /// set the sound to playback on this channel
    void _AttachSound(Sound* sound);
    /// set the internal channel index for a channel
    void _SetIndex(int index);
    /// set isplaying flag
    void _SetIsPlaying(bool isplaying);
    /// retrieve if properties (volume, panning ect.) has changed
    bool _PropertiesHasChanged();

private:
    Sound* currentSound;
    int index;
    unsigned int position;
    bool isPlaying;
    float volume;
    float panning;
    Mode mode;
    bool propertiesHasChanged;
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
