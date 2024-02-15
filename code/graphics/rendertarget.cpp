//------------------------------------------------------------------------------
//  rendertarget.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "rendertarget.h"
#include "chrisslyarray.h"

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
RenderTarget::RenderTarget() :
    viewports(NULL),
    buffer(NULL),
    width(0),
    height(0),
    format(PF_UNKNOWN),
    frameTime(0.0)
{
    ce_array_init(this->viewports, 1U);
    this->timer.Start();
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
void
RenderTarget::Update()
{
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->viewports); ++i)
    {
        this->viewports[i]->Update();
    }

    this->frameTime = this->timer.GetTime();
    this->timer.Reset();
}

//------------------------------------------------------------------------------
/**
*/
Viewport*
RenderTarget::AddViewport(Camera* const cam, int left, int top, int w, int h)
{
    Viewport* viewport = CE_NEW Viewport(cam, this, left, top, w, h);
    ce_array_push_back(this->viewports, viewport);

    return viewport;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
RenderTarget::GetNumViewports() const
{
    return (unsigned short)ce_array_size(this->viewports);
}

//------------------------------------------------------------------------------
/**
*/
Viewport*
RenderTarget::GetViewport(unsigned short index) const
{
    return this->viewports[index];
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTarget::RemoveAllViewports()
{
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->viewports); ++i)
    {
        CE_DELETE this->viewports[i];
    }
    ce_array_delete(this->viewports);
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
double
RenderTarget::GetFrameTime() const
{
    return this->frameTime;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTarget::ResetFrameTime()
{
    this->frameTime = 0.0;
    this->timer.Reset();
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
RenderTarget::GetType() const
{
    return 0U;
}

} // namespace graphics
} // namespace chrissly