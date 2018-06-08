#ifndef RENDERTEXTURE_H_
#define RENDERTEXTURE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::RenderTexture

    (C) 2011 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __CE_PSP__
#include "psp/psprendertexture.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::PSPRenderTexture RenderTexture;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __CE_GLES2__
#include "gles2/gles2rendertexture.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::GLES2RenderTexture RenderTexture;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __CE_D3D11__
#include "d3d11/d3d11rendertexture.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::D3D11RenderTexture RenderTexture;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "RenderTexture class not implemented on this platform!"
#endif
#endif
