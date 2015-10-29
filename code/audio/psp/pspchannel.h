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
    /// returns the semaphore id of this channel
    SceUID GetSemaphoreId() const;
    /// signal the channel thread to release the channel
    void RequestRelease();
    /// returns if a channel release was requested
    bool GetReleaseRequest();

private:
    SceUID semaphoreId;
    SceUID threadId;
    volatile bool requestRelease;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
