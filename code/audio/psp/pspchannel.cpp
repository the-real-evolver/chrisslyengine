//------------------------------------------------------------------------------
//  pspchannel.cpp
//  (C) 2015 Christian Bleicher
//------------------------------------------------------------------------------
#include "pspchannel.h"
#include "pspaudiorenderer.h"
#include "debug.h"
#include <string.h>
#include <pspthreadman.h>

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
PSPChannel::PSPChannel() :
    threadId(-1),
    requestRelease(false)
{
    uintptr_t addr = (uintptr_t)this;

    char str[32U];
    snprintf(str, 32U, "audioChannelThread%u", addr);
    this->threadId = sceKernelCreateThread(str, PSPAudioRenderer::ChannelThread, 0x12, 0x10000, 0U, NULL);
    CE_ASSERT(this->threadId >= 0, "PSPChannel::PSPChannel(): sceKernelCreateThread() failed: %08x\n", this->threadId);

    int error = sceKernelStartThread(this->threadId, sizeof(uintptr_t), &addr);
    CE_ASSERT(0 == error, "PSPChannel::PSPChannel(): sceKernelStartThread() failed: %08x\n", error);
}

//------------------------------------------------------------------------------
/**
*/
PSPChannel::~PSPChannel()
{
    int error = sceKernelTerminateDeleteThread(this->threadId);
    CE_ASSERT(error >= 0, "PSPChannel::~PSPChannel(): sceKernelDeleteThread() failed: %08x\n", error);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPChannel::RequestRelease()
{
    this->requestRelease = true;
}

//------------------------------------------------------------------------------
/**
*/
bool
PSPChannel::GetReleaseRequest()
{
    bool requestRelease = this->requestRelease;
    this->requestRelease = false;
    return requestRelease;
}

} // namespace chrissly
