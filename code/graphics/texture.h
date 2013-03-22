#ifndef TEXTURE_H_
#define TEXTURE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::Texture

    (C) 2010 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __PSP__
#include "texturebase.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::graphics::TextureBase Texture;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __ANDROID__
#include "android/gles2texture.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::GLES2Texture Texture;

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "Texture class not implemented on this platform!"
#endif
#endif
