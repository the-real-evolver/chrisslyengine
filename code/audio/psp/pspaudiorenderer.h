#ifndef PSPAUDIORENDERER_H_
#define PSPAUDIORENDERER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::PSPAudioRenderer

    (C) 2014 Christian Bleicher
*/
#include "channel.h"
#include <pspaudio.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class PSPAudioRenderer
{
public:
    /// get pointer to the singleton
    static PSPAudioRenderer* const Instance()
    {
        return Singleton;
    }

    /// default constructor
    PSPAudioRenderer();
    /// destructor
    ~PSPAudioRenderer();
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

    /// channel thread
    static int ChannelThread(SceSize args, void* argp);

private:
    /// copy constructor
    PSPAudioRenderer(const PSPAudioRenderer&cc) {};

    static PSPAudioRenderer* Singleton;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
