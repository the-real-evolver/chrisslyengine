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

int PSPChannel::ChannelNumber = 0;

//------------------------------------------------------------------------------
/**
*/
PSPChannel::PSPChannel()
{
    char str[23];
    strcpy(str, "audioChannelSemaphoreX");
    str[21] = '0' + ChannelNumber;
    this->semaphoreId = sceKernelCreateSema(str, 0, 1, 1, NULL);
    CE_ASSERT(this->semaphoreId >= 0, "PSPChannel::PSPChannel(): sceKernelCreateSema() failed: %08x\n", this->semaphoreId);

    strcpy(str, "audioChannelThreadX");
    str[18] = '0' + ChannelNumber;
    this->threadId = sceKernelCreateThread(str, PSPAudioRenderer::ChannelThread, 0x12, 0x10000, 0, NULL);
    CE_ASSERT(this->threadId >= 0, "PSPChannel::PSPChannel(): sceKernelCreateThread() failed: %08x\n", this->threadId);

    unsigned int addr = (unsigned int)this;
    int error = sceKernelStartThread(this->threadId, sizeof(unsigned int), &addr);
    CE_ASSERT(error == 0, "PSPChannel::PSPChannel(): sceKernelStartThread() failed: %08x\n", error);

    ChannelNumber++;
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

    ChannelNumber--;
}

//------------------------------------------------------------------------------
/**
*/
SceUID
PSPChannel::GetSemaphoreId() const
{
    return this->semaphoreId;
}

} // namespace chrissly
