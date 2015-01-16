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
public:
    /// constructor with channel number
    PSPChannel();
    /// destructor
    ~PSPChannel();
    /// returns the semaphore id of this channel
    SceUID GetSemaphoreId() const;
    /// signal the channel thread to stop playback
    void RequestStop();
    /// returns if the playback stop was requested
    bool GetStopRequest();

private:
    SceUID semaphoreId;
    SceUID threadId;
    volatile bool requestStop;

    static int ChannelNumber;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
