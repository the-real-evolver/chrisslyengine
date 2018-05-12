#ifndef RENDERWINDOW_H_
#define RENDERWINDOW_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::RenderWindow

    (C) 2011 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __PSP__
#include "psp/psprenderwindow.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::PSPRenderWindow RenderWindow;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __ANDROID__
#include "egl/eglrenderwindow.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::EGLRenderWindow RenderWindow;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __WINDOWS__
#include "d3d11/dxgirenderwindow.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::DXGIRenderWindow RenderWindow;

} // namespace graphics
} // namespace chrissly
  //------------------------------------------------------------------------------
#else
#error "RenderWindow class not implemented on this platform!"
#endif
#endif
