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
public:
    /// default constructor
    SLESSound();
    /// destructor
    ~SLESSound();

    /// create internal sound resources
    void _CreateInternalResources();
    /// gets the sl audio source
    SLDataSource GetAudioSource() const;

private:
    SLDataFormat_PCM pcm;
    SLDataLocator_AndroidSimpleBufferQueue bufferQueue;
    SLDataSource audioSource;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
