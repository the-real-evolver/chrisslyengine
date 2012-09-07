//------------------------------------------------------------------------------
//  gles2renderwindow.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "gles2renderwindow.h"
#include "core/debug.h"

namespace chrissly
{
//------------------------------------------------------------------------------
/**
*/
GLES2RenderWindow::GLES2RenderWindow()
{

}

//------------------------------------------------------------------------------
/**
*/
GLES2RenderWindow::GLES2RenderWindow(void* windowHandle)
{
    this->window = (EGLNativeWindowType)windowHandle;
    this->display = EGL_NO_DISPLAY;
    this->surface = EGL_NO_SURFACE;
    this->context = EGL_NO_CONTEXT;
}

//------------------------------------------------------------------------------
/**
*/
GLES2RenderWindow::~GLES2RenderWindow()
{
    this->RemoveAllViewports();
    
    if (this->display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(this->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (this->context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(this->display, this->context);
        }
        if (this->surface != EGL_NO_SURFACE)
        {
            eglDestroySurface(this->display, this->surface);
        }
        eglTerminate(this->display);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderWindow::Create()
{
    this->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(this->display, 0, 0);

    // Here specify the attributes of the desired configuration.
    // Below, we select an EGLConfig with at least 8 bits per color
    // component compatible with on-screen windows
    const EGLint attribs[] =
    {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };
    EGLint numConfigs;
    EGLConfig config;

    // pick the first EGLConfig that matches
    eglChooseConfig(this->display, attribs, &config, 1, &numConfigs);

    // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
    // guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
    // As soon as we picked a EGLConfig, we can safely reconfigure the
    // ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
    EGLint format;
    eglGetConfigAttrib(this->display, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(this->window, 0, 0, format);

    this->surface = eglCreateWindowSurface(this->display, config, this->window, NULL);
    
    // create an OpenGL ES 2.0 context
    const EGLint contextAttrs[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    }; 
    this->context = eglCreateContext(this->display, config, NULL, contextAttrs);

    if (EGL_FALSE == eglMakeCurrent(this->display, this->surface, this->surface, this->context))
    {
        CE_ASSERT(false, "Unable to eglMakeCurrent");
    }

    eglQuerySurface(this->display, this->surface, EGL_WIDTH, &this->width);
    eglQuerySurface(this->display, this->surface, EGL_HEIGHT, &this->height);
    
    this->buffer = NULL;
    this->format = graphics::PF_R8G8B8;
}
    
//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderWindow::Update()
{
    if (EGL_NO_DISPLAY == this->display)
    {
        return;
    }
    
    // update all viewports
    unsigned int index;
    for (index = 0; index < this->numViewports; index++)
    {
        graphics::Viewport* vp = (graphics::Viewport*)DynamicArrayGet(&this->viewportList, index);
        vp->Update();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderWindow::SwapBuffers()
{
    if (EGL_NO_DISPLAY == this->display)
    {
        return;
    }
    
    eglSwapBuffers(this->display, this->surface); 
}

} // namespace chrissly