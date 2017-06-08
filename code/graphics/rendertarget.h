#ifndef RENDERTARGET_H_
#define RENDERTARGET_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::RenderTarget

    (C) 2011 Christian Bleicher
*/
#include "dynamicarray.h"
#include "camera.h"
#include "rendertarget.h"
#include "pixelformat.h"
#include "timer.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{
class Viewport;

class RenderTarget
{
public:
    /// default constructor
    RenderTarget();
    /// destructor
    virtual ~RenderTarget();
    /// tells the target to update it's contents
    virtual void Update();
    /// swaps the frame buffers to display the next frame
    virtual void SwapBuffers() = 0;
    /// adds a viewport to the rendering target
    Viewport* const AddViewport(Camera* const cam, int left, int top, int w, int h);
    /// returns the number of viewports attached to this target
    unsigned short GetNumViewports() const;
    /// retrieves a pointer to the viewport with the given index
    Viewport* const GetViewport(unsigned short index) const;
    /// removes all viewports on this target
    void RemoveAllViewports();
    /// get the width of the rendertarget
    int GetWidth() const;
    /// get the height of the rendertarget
    int GetHeight() const;
    /// get the pixelformat of the buffer
    PixelFormat GetFormat() const;
    /// get pointer to buffer
    void* const GetBuffer() const;
    /// get the current frame time
    double GetFrameTime() const;

    /// get the type of the rendertarget
    virtual unsigned int GetType() const;

protected:
    mutable ce_dynamic_array viewports;
    void* buffer;
    int width;
    int height;
    PixelFormat format;
    core::Timer timer;
    double frameTime;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
