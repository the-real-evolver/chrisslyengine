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
#include "android/gles2renderwindow.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::GLES2RenderWindow RenderWindow;

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "RenderWindow class not implemented on this platform!"
#endif
#endif
