//------------------------------------------------------------------------------
//  rendertarget.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "rendertarget.h"

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
RenderTarget::RenderTarget() :
    buffer(NULL),
    width(0),
    height(0),
    format(PF_UNKNOWN)
{
    DynamicArrayInit(&this->viewportList, 1);
}

//------------------------------------------------------------------------------
/**
*/
RenderTarget::~RenderTarget()
{

}

//------------------------------------------------------------------------------
/**
*/
Viewport*
RenderTarget::AddViewport(Camera *cam, int left, int top, int width, int height)
{
    Viewport* viewport = CE_NEW Viewport(cam, this, left, top, width, height);
    DynamicArrayPushBack(&this->viewportList, viewport);

    return viewport;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
RenderTarget::GetNumViewports() const
{
    return this->viewportList.size;
}

//------------------------------------------------------------------------------
/**
*/
Viewport*
RenderTarget::GetViewport(unsigned short index) const
{
    return (Viewport*)DynamicArrayGet(&this->viewportList, index);
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTarget::RemoveAllViewports()
{
    unsigned int i;
    for (i = 0; i < this->viewportList.size; ++i)
    {
        CE_DELETE (Viewport*)DynamicArrayGet(&this->viewportList, i);
    }

    DynamicArrayDelete(&this->viewportList);
}

//------------------------------------------------------------------------------
/**
*/
int
RenderTarget::GetWidth() const
{
    return this->width;
}

//------------------------------------------------------------------------------
/**
*/
int
RenderTarget::GetHeight() const
{
    return this->height;
}

//------------------------------------------------------------------------------
/**
*/
PixelFormat
RenderTarget::GetFormat() const
{
    return this->format;
}

//------------------------------------------------------------------------------
/**
*/
void*
RenderTarget::GetBuffer() const
{
    return this->buffer;
}

} // namespace graphics
} // namespace chrissly