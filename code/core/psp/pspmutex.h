#ifndef PSPMUTEX_H_
#define PSPMUTEX_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::PSPMutex

    (C) 2015 Christian Bleicher
*/
#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <pspkerneltypes.h>
#include <pspthreadman.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class PSPMutex
{
public:
    /// constructor
    PSPMutex();
    /// destructor
    ~PSPMutex();
    /// lock mutex
    void Lock() const;
    /// unlock mutex
    void Unlock() const;

private:
    mutable SceUID semaphoreId;
};

//------------------------------------------------------------------------------
/**
*/
inline
PSPMutex::PSPMutex()
{
    char str[16];
    snprintf(str, 16, "Mutex%u", (unsigned int)this);
    this->semaphoreId = sceKernelCreateSema(str, 0, 1, 1, NULL);
    CE_ASSERT(this->semaphoreId >= 0, "PSPMutex::PSPMutex(): sceKernelCreateSema() failed: %08x\n", this->semaphoreId);
}

//------------------------------------------------------------------------------
/**
*/
inline
PSPMutex::~PSPMutex()
{
    int error = sceKernelDeleteSema(this->semaphoreId);
    CE_ASSERT(error == 0, "PSPMutex::~PSPMutex(): sceKernelDeleteSema() failed: %08x\n", error);
}

//------------------------------------------------------------------------------
/**
*/
inline void
PSPMutex::Lock() const
{
    int error = sceKernelWaitSema(this->semaphoreId, 1, 0);
    CE_ASSERT(error >= 0, "PSPMutex::Lock(): sceKernelWaitSema() failed: %08x\n", error);
}

//------------------------------------------------------------------------------
/**
*/
inline void
PSPMutex::Unlock() const
{
    int error = sceKernelSignalSema(this->semaphoreId, 1);
    CE_ASSERT(error >= 0, "PSPMutex::Unlock(): sceKernelSignalSema() failed: %08x\n", error);
}

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
