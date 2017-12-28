#ifndef SLESCHANNEL_H_
#define SLESCHANNEL_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::SLESChannel

    (C) 2014 Christian Bleicher
*/
#include "channelbase.h"
#include <SLES/OpenSLES.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class SLESChannel : public audio::ChannelBase
{
    friend class SLESAudioRenderer;
public:
    /// default constructor
    SLESChannel();
    /// destructor
    ~SLESChannel();

private:
    /// copy constructor
    SLESChannel(const SLESChannel&);
    /// prevent copy by assignment
    SLESChannel& operator = (const SLESChannel&);

    /// setup the sl audio player
    void SetupAudioPlayer(SLEngineItf engineInterface, SLObjectItf outputMix);
    /// stop and destroy the sl audio player
    void ReleaseAudioPlayer();
    /// gets the player interface
    SLPlayItf GetPlayerInterface() const;
    /// gets the buffer queue interface
    SLAndroidSimpleBufferQueueItf GetBufferQueueInterface() const;
    /// gets the volume interface
    SLVolumeItf GetVolumeInterface() const;
    /// gets the effect send interface
    SLEffectSendItf GetEffectSendInterface() const;

    SLAndroidSimpleBufferQueueItf bufferQueueInterface;
    SLPlayItf playerInterface;
    SLVolumeItf volumeInterface;
    SLObjectItf player;
    SLEffectSendItf effectSendInterface;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
