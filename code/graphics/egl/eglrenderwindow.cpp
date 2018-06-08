//------------------------------------------------------------------------------
//  eglrenderwindow.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "eglrenderwindow.h"
#include "core/debug.h"

namespace chrissly
{
//------------------------------------------------------------------------------
/**
*/
EGLRenderWindow::EGLRenderWindow()
{

}

//------------------------------------------------------------------------------
/**
*/
EGLRenderWindow::EGLRenderWindow(void* windowHandle) :
    window((EGLNativeWindowType)windowHandle),
    display(EGL_NO_DISPLAY),
    surface(EGL_NO_SURFACE),
    context(EGL_NO_CONTEXT)
{

}

//------------------------------------------------------------------------------
/**
*/
EGLRenderWindow::~EGLRenderWindow()
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
EGLRenderWindow::Create()
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

#if __CE_ANDROID__
    // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
    // guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
    // As soon as we picked a EGLConfig, we can safely reconfigure the
    // ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
    EGLint format;
    eglGetConfigAttrib(this->display, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(this->window, 0, 0, format);
#endif

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
        CE_ASSERT(false, "EGLRenderWindow::Create(): Unable to eglMakeCurrent\n");
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
EGLRenderWindow::SwapBuffers()
{
    eglSwapBuffers(this->display, this->surface);
}

} // namespace chrissly