#ifndef PSPAUDIORENDERER_H_
#define PSPAUDIORENDERER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::PSPAudioRenderer

    (C) 2014 Christian Bleicher
*/
#include "sound.h"
#include "channel.h"
#include <pspaudio.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class PSPAudioRenderer
{
public:
    /// get pointer to the singleton
    static PSPAudioRenderer* Instance()
    {
        return Singleton;
    }

    /// default constructor
    PSPAudioRenderer();
    /// destructor
    ~PSPAudioRenderer();
    /// start playback on the given channel
    void StartChannel(audio::Channel* channel);
    /// update playback on the given channel
    void UpdateChannel(audio::Channel* channel);
    /// release the given channel
    void ReleaseChannel(audio::Channel* channel);

private:
    /// copy constructor
    PSPAudioRenderer(const PSPAudioRenderer&cc) {};
    /// calculate left and right volume from volume and panning
    void CalculateVolumesFromPanning(float volume, float panning, int& leftVolume, int& rightVolume);
    /// return a PspAudioFormats equivalent for the given number of channels
    PspAudioFormats GetFormat(int channels);

    static PSPAudioRenderer* Singleton;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
