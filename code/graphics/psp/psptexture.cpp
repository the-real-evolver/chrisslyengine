//------------------------------------------------------------------------------
//  psptexture.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "psptexture.h"
#include "psphardwarebuffermanager.h"
#include "pspmappings.h"
#include "chrisslyarray.h"

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
PSPTexture::PSPTexture() :
    mipmapInfos(NULL)
{
}

//------------------------------------------------------------------------------
/**
*/
PSPTexture::PSPTexture(graphics::RenderTexture* const rt) :
    TextureBase(rt),
    mipmapInfos(NULL)
{
}

//------------------------------------------------------------------------------
/**
*/
PSPTexture::PSPTexture(void* faces[6U]) :
    TextureBase(faces),
    mipmapInfos(NULL)
{
}

//------------------------------------------------------------------------------
/**
*/
PSPTexture::~PSPTexture()
{
    if (!this->isRenderTarget)
    {
        ce_array_delete(this->mipmapInfos);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PSPTexture::CreateInternalResources()
{
    if (!this->isRenderTarget)
    {
        if (graphics::TEX_TYPE_CUBE_MAP == this->type) this->textureBuffer = this->cubeFaces[0U]; // workaround to avoid crash and at least render something (PSP does not support cubemaps)

        ce_array_init(this->mipmapInfos, this->numMipmaps);

        void* mipmapBuffer = this->textureBuffer;
        int mipmapWidth = this->width;
        int mipmapHeight = this->height;

        unsigned int i;
        for (i = 0U; i < ce_array_header(this->mipmapInfos)->capacity; ++i)
        {
            mipmapBuffer = (void*)((uintptr_t)mipmapBuffer + PSPHardwareBufferManager::GetMemorySize(mipmapWidth, mipmapHeight, PSPMappings::Get(this->format)));
            mipmapWidth = (unsigned int)mipmapWidth >> 1U;
            mipmapHeight = (unsigned int)mipmapHeight >> 1U;

            MipmapInfo mipmapInfo;
            mipmapInfo.buffer = mipmapBuffer;
            mipmapInfo.width = mipmapWidth;
            mipmapInfo.height = mipmapHeight;

            this->mipmapInfos[i] = mipmapInfo;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
PSPTexture::MipmapInfo* const
PSPTexture::GetMipmapInfo(int level) const
{
    return &this->mipmapInfos[level - 1];
}

} // namespace chrissly
