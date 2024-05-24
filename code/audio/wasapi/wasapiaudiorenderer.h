#ifndef WASAPIAUDIORENDERER_H_
#define WASAPIAUDIORENDERER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::WASAPIAudioRenderer

    (C) 2016 Christian Bleicher
*/
#include "channel.h"
#include "debug.h"
#include <mmdeviceapi.h>
#include <Audioclient.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class WASAPIAudioRenderer
{
public:
    /// get pointer to the singleton
    static WASAPIAudioRenderer* const Instance()
    {
        return Singleton;
    }

    /// default constructor
    WASAPIAudioRenderer();
    /// destructor
    ~WASAPIAudioRenderer();
    /// start up the renderer
    void Initialise(void* const customParams);
    /// shutdown the renderer and cleanup resources
    void Shutdown();
    /// starts calling the softwaremixer and sends mixed buffer to the output
    void StartAudioProcessing();
    /// stops the softwaremixer, close mixerthread etc.
    void StopAudioProcessing();
    /// gets the number of hardware channels
    unsigned short GetNumHardwareChannels() const;
    /// start playback on the given channel
    void StartChannel(audio::Channel* const channel);
    /// update playback on the given channel
    void UpdateChannel(audio::Channel* const channel);
    /// release the given channel
    void ReleaseChannel(audio::Channel* const channel);

    /// run the audio thread
    void RunAudioThread();

private:
    /// copy constructor
    WASAPIAudioRenderer(const WASAPIAudioRenderer&);
    /// prevent copy by assignment
    WASAPIAudioRenderer& operator = (const WASAPIAudioRenderer&);

    static WASAPIAudioRenderer* Singleton;

    bool running;
    HANDLE thread;
    core::Mutex syncLock;

    IMMDeviceEnumerator* enumerator;
    IMMDevice* device;
    IAudioClient* audioClient;
    IAudioRenderClient* renderClient;
    UINT32 bufferFrameCount;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
