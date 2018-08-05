#ifndef AUDIOSYSTEM_H_
#define AUDIOSYSTEM_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::AudioSystem

    (C) 2014 Christian Bleicher
*/
#include "sound.h"
#include "channel.h"
#include "audiorenderer.h"
#include "dynamicarray.h"
#include "vector3.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace audio
{

class AudioSystem
{
    friend class ChannelBase;
public:
    /// get pointer to the singleton
    static AudioSystem* const Instance()
    {
        return Singleton;
    }

    /// default constructor
    AudioSystem();
    /// destructor
    ~AudioSystem();
    /// initialises the system object, and the sound device. This has to be called at the start of the user's program
    Result Initialise(void* const customParams = NULL);
    /// closes and frees a system object and its resources
    Result Release();
    /// loads a sound into memory, or opens it for streaming
    Result CreateSound(const char* const name, Mode mode, Sound** sound);
    /// plays a sound object on a particular channel
    Result PlaySound(int channelid, Sound* const sound, bool paused, Channel** channel);
    /// updates the audio system. This should be called once per 'game' tick, or once per frame in your application
    Result Update();
    /// updates the position and orientation of the 3D sound listener
    Result Set3DListenerAttributes(const core::Vector3* const pos, const core::Vector3* const forward, const core::Vector3* const up);

    /// software mix all currently active channels to 16 bit stereo output
    void _Mix(unsigned int numSamples, unsigned char* const buffer);

private:
    /// copy constructor
    AudioSystem(const AudioSystem&);
    /// prevent copy by assignment
    AudioSystem& operator = (const AudioSystem&);
    /// internal method to retrieve a pointer to the currently selected audiorenderer
    AudioRenderer* const GetAudioRenderer() const;

    static AudioSystem* Singleton;

    AudioRenderer* activeRenderer;

    ce_dynamic_array soundPool;

    ce_dynamic_array channelPool;

    core::Vector3 listenerPos;
    core::Vector3 listenerForward;
    core::Vector3 listenerUp;
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
