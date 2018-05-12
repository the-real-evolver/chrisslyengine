#ifndef EGLRENDERWINDOW_H_
#define EGLRENDERWINDOW_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::EGLRenderWindow

    (C) 2012 Christian Bleicher
*/
#include "rendertarget.h"
#include <EGL/egl.h>

//------------------------------------------------------------------------------
namespace chrissly
{
class EGLRenderWindow : public graphics::RenderTarget
{
public:
    /// constructor with window handle
    EGLRenderWindow(void* windowHandle);
    /// destructor
    ~EGLRenderWindow();
    /// creates & displays the new window
    void Create();
    /// swaps the frame buffers to display the next frame
    void SwapBuffers();

private:
    /// private default constructor
    EGLRenderWindow();

    EGLNativeWindowType window;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
