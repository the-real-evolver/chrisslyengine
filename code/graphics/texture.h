#ifndef TEXTURE_H_
#define TEXTURE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::Texture

    (C) 2010 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __CE_PSP__
#include "psp/psptexture.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::PSPTexture Texture;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __CE_GLES2__
#include "gles2/gles2texture.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::GLES2Texture Texture;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __CE_D3D11__
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
