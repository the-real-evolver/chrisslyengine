//------------------------------------------------------------------------------
//  gles2rendertexture.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "gles2rendertexture.h"

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
GLES2RenderTexture::GLES2RenderTexture()
{

}

//------------------------------------------------------------------------------
/**
*/
GLES2RenderTexture::~GLES2RenderTexture()
{
    this->RemoveAllViewports();
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderTexture::Create(int width, int height, graphics::PixelFormat format)
{
    this->width = width;
    this->height = height;
    this->format = format;
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderTexture::Update()
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
GLES2RenderTexture::SwapBuffers()
{
    // do nothing no need for doublebuffering
}

} // namespace chrissly