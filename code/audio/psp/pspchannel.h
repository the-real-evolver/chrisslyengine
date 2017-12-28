#ifndef PSPCHANNEL_H_
#define PSPCHANNEL_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::PSPChannel

    (C) 2015 Christian Bleicher
*/
#include "channelbase.h"
#include <pspkerneltypes.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class PSPChannel : public audio::ChannelBase
{
    friend class PSPAudioRenderer;
public:
    /// constructor
    PSPChannel();
    /// destructor
    ~PSPChannel();

private:
    /// copy constructor
    PSPChannel(const PSPChannel&);
    /// prevent copy by assignment
    PSPChannel& operator = (const PSPChannel&);

    /// returns the id of the kernel event used to synchronise the shutdown of the audio output
    SceUID GetShutdownEvent() const;
    /// returns if the channel is running
    bool IsRunning() const;
    /// signal the channel thread to release the channel
    void RequestRelease();
    /// returns if a channel release was requested
    bool GetReleaseRequest();

    SceUID shutdownEvent;
    bool running;
    SceUID threadId;
    bool requestRelease;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
