#ifndef SLESAUDIORENDERER_H_
#define SLESAUDIORENDERER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::SLESAudioRenderer

    (C) 2014 Christian Bleicher
*/
#include "channel.h"
#include "debug.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class SLESAudioRenderer
{
public:
    /// get pointer to the singleton
    static SLESAudioRenderer* Instance()
    {
        return Singleton;
    }

    /// default constructor
    SLESAudioRenderer();
    /// destructor
    ~SLESAudioRenderer();
    /// start up the renderer
    void _Initialise(void* customParams);
    /// shutdown the renderer and cleanup resources
    void Shutdown();
    /// starts calling the softwaremixer and sends mixed buffer to the output
    void StartAudioProcessing();
    /// stops the softwaremixer, close mixerthread etc.
    void StopAudioProcessing();
    /// gets the number of hardware channels
    unsigned short GetNumHardwareChannels() const;
    /// start playback on the given channel
    void StartChannel(audio::Channel* channel);
    /// update playback on the given channel
    void UpdateChannel(audio::Channel* channel);
    /// release the given channel
    void ReleaseChannel(audio::Channel* channel);

    /// this callback handler is called every time a buffer finishes playing
    static void BufferQueueCallback(SLAndroidSimpleBufferQueueItf bufferQueueInterface, void* context);

private:
    /// copy constructor
    SLESAudioRenderer(const SLESAudioRenderer&cc) {};

    static SLESAudioRenderer* Singleton;

    SLint16 numHardwareChannels;
    SLObjectItf engine;
    SLEngineItf engineInterface;
    SLObjectItf outputMix;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
