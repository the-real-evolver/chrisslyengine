#ifndef CHANNELBASE_H_
#define CHANNELBASE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::ChannelBase

    (C) 2014 Christian Bleicher
*/
#include "sound.h"
#include "dsp.h"
#include "propertychangedflags.h"
#include "vector3.h"
#include "chrisslymutex.h"
#include "dynamicarray.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace audio
{

class ChannelBase
{
    friend class AudioSystem;
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
    /// stops the channel from playing, makes it available for re-use
    Result Stop();
    /// returns the playing state for the current channel
    Result IsPlaying(bool* const isplaying);
    /// sets the paused state of the channel
    Result SetPaused(bool pause);
    /// retrieves the paused state of the channel
    Result GetPaused(bool* const pause);
    /// sets the volume for the channel linearly (reaches from 0.0 to 1.0 inclusive. 0.0 = silent, 1.0 = full volume. Default = 1.0)
    Result SetVolume(float vol);
    /// retrieves the volume level for the channel
    Result GetVolume(float* const vol);
    /// set sound pressure level relative to a reference value of 20 micropascal which is often considered as the "absolute threshold of hearing" (reaches from -32.0 to 0.0 inclusive. -32.0 = silent, 0.0 = full)
    Result SetPressureLevel(float decibel);
    /// sets a channels pan position linearly (a left/right pan level, from -1.0 to 1.0 inclusive. -1.0 = Full left, 0.0 = center, 1.0 = full right. Default = 0.0)
    Result SetPan(float pan);
    /// returns the pan position of the channel
    Result GetPan(float* const pan);
    /// changes some attributes for a channel based on the mode passed in
    Result SetMode(Mode modeflags);
    /// retrieves the current mode bit flags for the current channel
    Result GetMode(Mode* const modeflags);
    /// sets the current playback position for the currently playing sound to the specified PCM offset
    Result SetPosition(unsigned int position);
    /// returns the current PCM offset or playback position for the specified channel
    Result GetPosition(unsigned int* const position);
    /// sets the position of a 3d channel
    Result Set3DAttributes(const core::Vector3* const position);
    /// retrieves the position of a 3d channel
    Result Get3DAttributes(core::Vector3* const position);
    /// sets the minimum and maximum audible distance for a channel
    Result Set3DMinMaxDistance(float mindistance, float maxdistance);
    /// retrieves the current minimum and maximum audible distance for a channel
    Result Get3DMinMaxDistance(float* const mindistance, float* const maxdistance);
    /// returns the currently playing sound for this channel
    Result GetCurrentSound(Sound** sound);
    /// retrieves the internal channel index for a channel
    Result GetIndex(int* const idx);
    /// adds a dsp to the channel
    Result AddDSP(unsigned int idx, DSP* const dsp);

protected:
    /// copy constructor
    ChannelBase(const ChannelBase&);
    /// prevent copy by assignment
    ChannelBase& operator = (const ChannelBase&);
    /// set the sound to playback on this channel
    void AttachSound(Sound* const sound);
    /// set the internal channel index for a channel
    void SetIndex(int idx);
    /// set isplaying flag
    void SetIsPlaying(bool isplaying);
    /// set the factor the volume will be scaled with internal if the channel is playing in 3d mode
    void SetAttenuationFactor(float attenuation);
    /// get the internal factor the volume will be scaled with if the channel is playing in 3d mode
    float GetAttenuationFactor() const;
    /// retrieve if properties (volume, panning etc.) have changed
    PropertyChange PropertiesHaveChanged();
    /// gets the sync lock mutex
    const core::Mutex& GetSyncLock() const;
    /// processes all attached dsps and returns the mixed result, otherwise the 'dry' sample or stream buffer is returned
    void* const FillOutputBuffer(unsigned int numSamples, unsigned int position);
    /// called when the hardware channel has finished playing or was stopped explicitly
    void ReleaseInternal();

    bool isPlaying;
    bool paused;
    float volume;
    float panning;
    Mode mode;
    unsigned int samplePosition;
    core::Vector3 spatialPosition;
    float minDistance;
    float maxDistance;
    Sound* currentSound;
    int index;
    float attenuationFactor;
    PropertyChange propertiesHaveChanged;
    core::Mutex syncLock;
    int outputBuffer[1024];
    ce_dynamic_array dsps;
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
