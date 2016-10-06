//------------------------------------------------------------------------------
//  rendertarget.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "rendertarget.h"
#include "debug.h"

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
    ce_dynamic_array_init(&this->viewports, 1);
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
RenderTarget::AddViewport(Camera *cam, int left, int top, int w, int h)
{
    Viewport* viewport = CE_NEW Viewport(cam, this, left, top, w, h);
    ce_dynamic_array_push_back(&this->viewports, viewport);

    return viewport;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
RenderTarget::GetNumViewports() const
{
    return (unsigned short)this->viewports.size;
}

//------------------------------------------------------------------------------
/**
*/
Viewport*
RenderTarget::GetViewport(unsigned short index) const
{
    return (Viewport*)ce_dynamic_array_get(&this->viewports, index);
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTarget::RemoveAllViewports()
{
    unsigned int i;
    for (i = 0; i < this->viewports.size; ++i)
    {
        CE_DELETE (Viewport*)ce_dynamic_array_get(&this->viewports, i);
    }

    ce_dynamic_array_delete(&this->viewports);
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

//------------------------------------------------------------------------------
/**
*/
unsigned int
RenderTarget::GetType() const
{
    return 0;
}

} // namespace graphics
} // namespace chrissly