#ifndef RENDERSYSTEM_H_
#define RENDERSYSTEM_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::RenderSystem

    (C) 2012 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __CE_PSP__
#include "psp/psprendersystem.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::PSPRenderSystem RenderSystem;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __CE_GLES2__
#include "gles2/gles2rendersystem.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::GLES2RenderSystem RenderSystem;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __CE_D3D11__
#include "d3d11/d3d11rendersystem.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::D3D11RenderSystem RenderSystem;

} // namespace graphics
} // namespace chrissly
  //------------------------------------------------------------------------------
#else
#error "RenderSystem class not implemented on this platform!"
#endif
#endif
