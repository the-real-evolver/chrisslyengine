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

static const int ThreadInitialPriority = 0x00000012;
static const int ThreadStackSize = 0x00010000;

//------------------------------------------------------------------------------
/**
*/
PSPChannel::PSPChannel() :
    shutdownEvent(-1),
    running(true),
    threadId(-1),
    requestRelease(false)
{
    char str[32U];
    uintptr_t addr = (uintptr_t)this;

    snprintf(str, 32U, "ShutdownEvent%u", addr);
    this->shutdownEvent = sceKernelCreateEventFlag(str, 0, 0, NULL);
    CE_ASSERT(this->shutdownEvent >= 0, "PSPChannel::PSPChannel(): sceKernelCreateEventFlag() failed: %08x\n", this->shutdownEvent);

    snprintf(str, 32U, "AudioChannelThread%u", addr);
    this->threadId = sceKernelCreateThread(str, PSPAudioRenderer::ChannelThread, ThreadInitialPriority, ThreadStackSize, THREAD_ATTR_VFPU, NULL);
    CE_ASSERT(this->threadId >= 0, "PSPChannel::PSPChannel(): sceKernelCreateThread() failed: %08x\n", this->threadId);

    int error = sceKernelStartThread(this->threadId, sizeof(uintptr_t), &addr);
    CE_ASSERT(0 == error, "PSPChannel::PSPChannel(): sceKernelStartThread() failed: %08x\n", error);
}

//------------------------------------------------------------------------------
/**
*/
PSPChannel::~PSPChannel()
{
    this->running = false;
    int error = sceKernelWaitEventFlag(this->shutdownEvent, PSPAudioRenderer::ShutdownEventFlag, PSP_EVENT_WAITAND, NULL, NULL);
    CE_ASSERT(error >= 0, "PSPChannel::~PSPChannel(): sceKernelWaitEventFlag() failed: %08x\n", error);
    error = sceKernelDeleteEventFlag(this->shutdownEvent);
    CE_ASSERT(error >= 0, "PSPChannel::~PSPChannel(): sceKernelDeleteEventFlag() failed: %08x\n", error);

    error = sceKernelTerminateDeleteThread(this->threadId);
    CE_ASSERT(error >= 0, "PSPChannel::~PSPChannel(): sceKernelDeleteThread() failed: %08x\n", error);
}

//------------------------------------------------------------------------------
/**
*/
SceUID
PSPChannel::GetShutdownEvent() const
{
    return this->shutdownEvent;
}

//------------------------------------------------------------------------------
/**
*/
bool
PSPChannel::IsRunning() const
{
    return this->running;
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
