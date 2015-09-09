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
PSPChannel::PSPChannel()
{
    this->requestRelease = false;

    unsigned int addr = (unsigned int)this;

    char str[32];
    snprintf(str, 32, "audioChannelSemaphore%u", addr);
    this->semaphoreId = sceKernelCreateSema(str, 0, 1, 1, NULL);
    CE_ASSERT(this->semaphoreId >= 0, "PSPChannel::PSPChannel(): sceKernelCreateSema() failed: %08x\n", this->semaphoreId);

    snprintf(str, 32, "audioChannelThread%u", addr);
    this->threadId = sceKernelCreateThread(str, PSPAudioRenderer::ChannelThread, 0x12, 0x10000, 0, NULL);
    CE_ASSERT(this->threadId >= 0, "PSPChannel::PSPChannel(): sceKernelCreateThread() failed: %08x\n", this->threadId);

    int error = sceKernelStartThread(this->threadId, sizeof(unsigned int), &addr);
    CE_ASSERT(error == 0, "PSPChannel::PSPChannel(): sceKernelStartThread() failed: %08x\n", error);
}

//------------------------------------------------------------------------------
/**
*/
PSPChannel::~PSPChannel()
{
    int error = sceKernelDeleteSema(this->semaphoreId);
    CE_ASSERT(error == 0, "PSPChannel::~PSPChannel(): sceKernelDeleteSema() failed: %08x\n", error);

    error = sceKernelTerminateDeleteThread(this->threadId);
    CE_ASSERT(error >= 0, "PSPChannel::~PSPChannel(): sceKernelDeleteThread() failed: %08x\n", error);
}

//------------------------------------------------------------------------------
/**
*/
SceUID
PSPChannel::GetSemaphoreId() const
{
    return this->semaphoreId;
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
