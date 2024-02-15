//------------------------------------------------------------------------------
//  viewport.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "viewport.h"
#include "rendertarget.h"

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
Viewport::Viewport(Camera* const camera, RenderTarget* const target, int left, int top, int width, int height) :
    camera(camera),
    target(target),
    actLeft(left),
    actTop(top),
    actWidth(width),
    actHeight(height),
    backColour(0xff554433),
    clearEveryFrame(true),
    clearBuffers(FBT_COLOUR | FBT_DEPTH)
{

}

//------------------------------------------------------------------------------
/**
*/
Viewport::~Viewport()
{

}

//------------------------------------------------------------------------------
/**
*/
void
Viewport::Update()
{
    if (this->camera != NULL)
    {
        // tell camera to render into me
        this->camera->_RenderScene(this);
    }
}

//------------------------------------------------------------------------------
/**
*/
RenderTarget*
Viewport::GetTarget() const
{
    return this->target;
}

//------------------------------------------------------------------------------
/**
*/
int
Viewport::GetActualLeft() const
{
    return this->actLeft;
}

//------------------------------------------------------------------------------
/**
*/
int
Viewport::GetActualTop() const
{
    return this->actTop;
}

//------------------------------------------------------------------------------
/**
*/
int
Viewport::GetActualWidth() const
{
    return this->actWidth;
}

//------------------------------------------------------------------------------
/**
*/
int
Viewport::GetActualHeight() const
{
    return this->actHeight;
}

//------------------------------------------------------------------------------
/**
*/
void
Viewport::SetBackgroundColour(unsigned int colour)
{
    this->backColour = colour;
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
Viewport::GetBackgroundColour() const
{
    return this->backColour;
}

//------------------------------------------------------------------------------
/**
*/
void
Viewport::SetClearEveryFrame(bool clear, unsigned int buffers)
{
    this->clearEveryFrame = clear;
    this->clearBuffers = buffers;
}

//------------------------------------------------------------------------------
/**
*/
bool
Viewport::GetClearEveryFrame() const
{
    return this->clearEveryFrame;
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
Viewport::GetClearBuffers() const
{
    return this->clearBuffers;
}

} // namespace graphics
} // namespace chrissly