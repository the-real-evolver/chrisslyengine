#ifndef PSPAUDIORENDERER_H_
#define PSPAUDIORENDERER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::PSPAudioRenderer

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
    /// describes the panning mode used during playback on a channel
    enum PanningMode
    {
        /// linear having at least one channel at full volume -1.0 = full left muted right, 0.0 = full left and right, 1.0 = full right muted left
        PAN_CLAMPEDLINEAR,
        /// constant power pan = 0.0, the balance for the sound in each speaker is 71% left and 71% right
        PAN_CONSTANTPOWER
    };

    /// get pointer to the singleton
    static PSPAudioRenderer* Instance()
    {
        return Singleton;
    }

    /// default constructor
    PSPAudioRenderer();
    /// destructor
    ~PSPAudioRenderer();
    /// start up the renderer
    void _Initialise(void* customParams);
    /// shutdown the renderer and cleanup resources
    void Shutdown();
    /// gets the number of hardware channels
    unsigned short GetNumHardwareChannels() const;
    /// start playback on the given channel
    void StartChannel(audio::Channel* channel);
    /// update playback on the given channel
    void UpdateChannel(audio::Channel* channel);
    /// release the given channel
    void ReleaseChannel(audio::Channel* channel);

    /// calculate left and right volume from volume and panning
    void CalculateVolumesFromPanning(PanningMode mode, float volume, float panning, int& leftVolume, int& rightVolume);
    /// return a PspAudioFormats equivalent for the given number of channels
    PspAudioFormats GetFormat(int channels) const;

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
