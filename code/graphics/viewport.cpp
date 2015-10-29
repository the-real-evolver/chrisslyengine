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
Viewport::Viewport(Camera* camera, RenderTarget* target, int left, int top, int width, int height)
{
    this->camera = camera;
    this->target = target;
    this->actLeft = left;
    this->actTop = top;
    this->actWidth = width;
    this->actHeight = height;

    this->clearEveryFrame = true;
    this->clearBuffers = FBT_COLOUR | FBT_DEPTH;
    this->backColour = 0xff554433;
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
    if (this->camera)
    {
        // Tell Camera to render into me
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
const unsigned int
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