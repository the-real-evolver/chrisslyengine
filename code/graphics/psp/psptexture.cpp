//------------------------------------------------------------------------------
//  psptexture.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "psptexture.h"
#include "psphardwarebuffermanager.h"
#include "pspmappings.h"

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
PSPTexture::PSPTexture()
{
    ce_dynamic_array_init(&this->mipmapInfos, 0U);
}

//------------------------------------------------------------------------------
/**
*/
PSPTexture::~PSPTexture()
{
    unsigned int i;
    for (i = 0U; i < this->mipmapInfos.capacity; ++i)
    {
        CE_DELETE (MipmapInfo*)ce_dynamic_array_get(&this->mipmapInfos, i);
    }

    ce_dynamic_array_delete(&this->mipmapInfos);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPTexture::CreateInternalResources()
{
    ce_dynamic_array_init(&this->mipmapInfos, this->numMipmaps);

    void* mipmapbuffer = this->textureBuffer;
    int mipmapWidth = this->width;
    int mipmapHeight = this->height;

    unsigned int i;
    for (i = 0U; i < this->mipmapInfos.capacity; ++i)
    {
        mipmapbuffer = (void*)((uintptr_t)mipmapbuffer + PSPHardwareBufferManager::GetMemorySize(mipmapWidth, mipmapHeight, PSPMappings::Get(this->format)));
        mipmapWidth = (unsigned int)mipmapWidth >> 1U;
        mipmapHeight = (unsigned int)mipmapHeight >> 1U;

        MipmapInfo* mipmapInfo = CE_NEW MipmapInfo;
        mipmapInfo->buffer = mipmapbuffer;
        mipmapInfo->width = mipmapWidth;
        mipmapInfo->height = mipmapHeight;

        ce_dynamic_array_set(&this->mipmapInfos, i, mipmapInfo);
    }
}

//------------------------------------------------------------------------------
/**
*/
PSPTexture::MipmapInfo* const
PSPTexture::GetMipmapInfo(int level) const
{
    return (MipmapInfo*)ce_dynamic_array_get(&this->mipmapInfos, level - 1);
}

} // namespace chrissly
