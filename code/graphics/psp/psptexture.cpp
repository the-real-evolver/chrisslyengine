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

}

//------------------------------------------------------------------------------
/**
*/
PSPTexture::~PSPTexture()
{
    unsigned int i;
    for (i = 0; i < this->mipmapInfos.capacity; ++i)
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
    for (i = 0; i < this->mipmapInfos.capacity; ++i)
    {
        mipmapbuffer = (void*)((unsigned int)mipmapbuffer + PSPHardwareBufferManager::GetMemorySize(mipmapWidth, mipmapHeight, PSPMappings::Get(this->format)));
        mipmapWidth = mipmapWidth >> 1;
        mipmapHeight = mipmapHeight >> 1;

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
PSPTexture::MipmapInfo*
PSPTexture::GetMipmapInfo(int level)
{
    return (MipmapInfo*)ce_dynamic_array_get(&this->mipmapInfos, level - 1);
}

} // namespace chrissly
