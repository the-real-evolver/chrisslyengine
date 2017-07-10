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
    /// signal the channel thread to release the channel
    void RequestRelease();
    /// returns if a channel release was requested
    bool GetReleaseRequest();

private:
    /// copy constructor
    PSPChannel(const PSPChannel&);
    /// prevent copy by assignment
    PSPChannel& operator = (const PSPChannel&);

    SceUID threadId;
    bool requestRelease;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
