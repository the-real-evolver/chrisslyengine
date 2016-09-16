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
    virtual void Update() = 0;
    /// swaps the frame buffers to display the next frame
    virtual void SwapBuffers() = 0;
    /// adds a viewport to the rendering target
    Viewport* AddViewport(Camera *cam, int left, int top, int w, int h);
    /// returns the number of viewports attached to this target
    unsigned short GetNumViewports() const;
    /// retrieves a pointer to the viewport with the given index
    Viewport* GetViewport(unsigned short index) const;
    /// removes all viewports on this target
    void RemoveAllViewports();
    /// get the width of the rendertarget
    int GetWidth() const;
    /// get the height of the rendertarget
    int GetHeight() const;
    /// get the pixelformat of the buffer
    PixelFormat GetFormat() const;
    /// get pointer to buffer
    void* GetBuffer() const;

    /// render target type - default is RT_NONE
    enum RenderTargetType
    {
        RT_NONE,
        RT_WINDOW,
        RT_TETXTURE
    };
    /// get the type of the rendertarget
    virtual RenderTargetType GetType() const;

protected:
    mutable ce_dynamic_array viewports;

    void* buffer;
    int width;
    int height;
    PixelFormat format;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
