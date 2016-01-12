//------------------------------------------------------------------------------
//  psprendertexture.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "psprendertexture.h"
#include "psphardwarebuffermanager.h"
#include "pspmappings.h"

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
PSPRenderTexture::PSPRenderTexture()
{

}

//------------------------------------------------------------------------------
/**
*/
PSPRenderTexture::~PSPRenderTexture()
{
    this->RemoveAllViewports();
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderTexture::Create(int width, int height, graphics::PixelFormat format)
{
    this->buffer = PSPHardwareBufferManager::GetStaticVramTexture(width, height, chrissly::PSPMappings::Get(format));
    this->width = width;
    this->height = height;
    this->format = format;
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderTexture::Update()
{
    // update all viewports
    unsigned int index;
    for (index = 0; index < this->numViewports; ++index)
    {
        graphics::Viewport* vp = (graphics::Viewport*)DynamicArrayGet(&this->viewportList, index);
        vp->Update();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderTexture::SwapBuffers()
{
    // do nothing no need for doublebuffering
}

} // namespace chrissly