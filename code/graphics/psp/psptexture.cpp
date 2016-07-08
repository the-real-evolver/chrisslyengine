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
        CE_DELETE (MipmapInfo*)DynamicArrayGet(&this->mipmapInfos, i);
    }

    DynamicArrayDelete(&this->mipmapInfos);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPTexture::CreateInternalResources()
{
    DynamicArrayInit(&this->mipmapInfos, this->numMipmaps);

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

        DynamicArraySet(&this->mipmapInfos, i, mipmapInfo);
    }
}

//------------------------------------------------------------------------------
/**
*/
PSPTexture::MipmapInfo*
PSPTexture::GetMipmapInfo(int level)
{
    return (MipmapInfo*)DynamicArrayGet(&this->mipmapInfos, level - 1);
}

} // namespace chrissly
