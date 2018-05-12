#ifndef TEXTURE_H_
#define TEXTURE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::Texture

    (C) 2010 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __PSP__
#include "psp/psptexture.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::PSPTexture Texture;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __ANDROID__
#include "gles2/gles2texture.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::GLES2Texture Texture;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __WINDOWS__
#include "d3d11/d3d11texture.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::D3D11Texture Texture;

} // namespace graphics
} // namespace chrissly
  //------------------------------------------------------------------------------
#else
#error "Texture class not implemented on this platform!"
#endif
#endif
