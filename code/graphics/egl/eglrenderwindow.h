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
    /// get the type of the rendertarget
    unsigned int GetType() const;
    /// gets the window handle
    EGLNativeWindowType GetWindowHandle() const;
    /// returns whether msaa is supported and enabled
    bool IsMSAAEnabled() const;

private:
    /// private default constructor
    EGLRenderWindow();

    EGLNativeWindowType window;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    bool msaaEnable;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
