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
PSPTexture::PSPTexture() :
    numMipmapInfos(0)
{

}

//------------------------------------------------------------------------------
/**
*/
PSPTexture::~PSPTexture()
{
    int i;
    for (i = 0; i < this->numMipmapInfos; ++i)
    {
        CE_DELETE (MipmapInfo*)DynamicArrayGet(&this->mipmapInfos, i);
    }

    DynamicArrayDelete(&this->mipmapInfos);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPTexture::CreateInternalResourcesImpl()
{
    DynamicArrayInit(&this->mipmapInfos, this->numMipmaps);
    this->numMipmapInfos = this->numMipmaps;

    void* mipmapbuffer = this->textureBuffer;
    int mipmapWidth = this->width;
    int mipmapHeight = this->height;

    int i;
    for (i = 0; i < this->numMipmaps; ++i)
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
