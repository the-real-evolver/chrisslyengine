#ifndef RENDERSYSTEM_H_
#define RENDERSYSTEM_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::RenderSystem

    (C) 2012 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __PSP__
#include "psp/psprendersystem.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::PSPRenderSystem RenderSystem;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __ANDROID__
#include "android/gles2rendersystem.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::GLES2RenderSystem RenderSystem;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "RenderSystem class not implemented on this platform!"
#endif
#endif
