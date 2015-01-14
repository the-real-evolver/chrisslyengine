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

private:
    SceUID semaphoreId;
    SceUID threadId;

    static int ChannelNumber;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
