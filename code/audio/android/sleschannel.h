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
public:
    /// default constructor
    SLESChannel();
    /// destructor
    ~SLESChannel();

    /// setup the sl audio player
    void SetupAudioPlayer(SLEngineItf engineInterface, SLObjectItf outputMix);
    /// stop and destroy the sl audio player
    void Release();
    /// gets the player interface
    SLPlayItf GetPlayerInterface() const;
    /// gets the player interface
    SLAndroidSimpleBufferQueueItf GetBufferQueueInterface() const;
    /// gets the volume interface
    SLVolumeItf GetVolumeInterface() const;

private:
    SLAndroidSimpleBufferQueueItf bufferQueueInterface;
    SLPlayItf playerInterface;
    SLVolumeItf volumeInterface;
    SLObjectItf player;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
