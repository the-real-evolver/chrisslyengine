#ifndef RENDERTEXTURE_H_
#define RENDERTEXTURE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::RenderTexture

    (C) 2011 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __PSP__
#include "psp/psprendertexture.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::PSPRenderTexture RenderTexture;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __ANDROID__
#include "android/gles2rendertexture.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::GLES2RenderTexture RenderTexture;

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "RenderTexture class not implemented on this platform!"
#endif
#endif
