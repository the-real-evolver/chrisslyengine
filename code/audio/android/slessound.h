#ifndef SLESSOUND_H_
#define SLESSOUND_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::SLESSound

    (C) 2014 Christian Bleicher
*/
#include "soundbase.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class SLESSound : public audio::SoundBase
{
    friend class SLESAudioRenderer;
    friend class SLESChannel;
public:
    /// default constructor
    SLESSound();
    /// destructor
    ~SLESSound();

private:
    /// copy constructor
    SLESSound(const SLESSound&);
    /// prevent copy by assignment
    SLESSound& operator = (const SLESSound&);
    /// create internal sound resources
    void CreateInternalResources();
    /// gets the sl audio source
    SLDataSource GetAudioSource() const;

    SLDataFormat_PCM pcm;
    SLDataLocator_AndroidSimpleBufferQueue bufferQueue;
    SLDataSource audioSource;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
