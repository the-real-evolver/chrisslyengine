//------------------------------------------------------------------------------
//  psphardwarebuffermanager.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "psphardwarebuffermanager.h"
#include "debug.h"
#include <pspge.h>
#include <pspgu.h>

namespace chrissly
{

uintptr_t PSPHardwareBufferManager::StaticOffset = 0U;

//------------------------------------------------------------------------------
/**
*/
unsigned int
PSPHardwareBufferManager::GetMemorySize(unsigned int width, unsigned int height, unsigned int psm)
{
    switch (psm)
    {
        case GU_PSM_T4:
            return (width * height) >> 1U;

        case GU_PSM_T8:
            return width * height;

        case GU_PSM_5650:
        case GU_PSM_5551:
        case GU_PSM_4444:
        case GU_PSM_T16:
            return 2U * width * height;

        case GU_PSM_8888:
        case GU_PSM_T32:
            return 4U * width * height;

        default:
            CE_ASSERT(false, "PSPHardwareBufferManager::GetMemorySize(): illegal psm format '%u'\n", psm);
    }

    return 0U;
}

//------------------------------------------------------------------------------
/**
*/
void* const
PSPHardwareBufferManager::GetStaticVramBuffer(unsigned int width, unsigned int height, unsigned int psm)
{
    void* result = (void*)StaticOffset;
    StaticOffset += GetMemorySize(width, height, psm);

    return result;
}

//------------------------------------------------------------------------------
/**
*/
void* const
PSPHardwareBufferManager::GetStaticVramTexture(unsigned int width, unsigned int height, unsigned int psm)
{
    void* result = GetStaticVramBuffer(width, height, psm);
    return (void*)(((uintptr_t)result) + ((uintptr_t)sceGeEdramGetAddr()));
}

//------------------------------------------------------------------------------
/**
*/
void
PSPHardwareBufferManager::Release()
{
    PSPHardwareBufferManager::StaticOffset = 0U;
}

} // namespace chrissly