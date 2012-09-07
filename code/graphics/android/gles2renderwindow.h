#ifndef GLES2RENDERWINDOW_H_
#define GLES2RENDERWINDOW_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::GLES2RenderWindow

    (C) 2012 Christian Bleicher
*/
#include "rendertarget.h"
#include <EGL/egl.h>

//------------------------------------------------------------------------------
namespace chrissly
{
class GLES2RenderWindow : public graphics::RenderTarget
{
public:
    /// constructor with window handle
    GLES2RenderWindow(void* windowHandle);
    /// destructor
    ~GLES2RenderWindow();
    /// creates & displays the new window
    void Create(); 
    /// tells the window to update it's contents
    void Update();
    /// swaps the frame buffers to display the next frame
    void SwapBuffers();

private:
    /// private default constructor
    GLES2RenderWindow();
    
    EGLNativeWindowType window;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
