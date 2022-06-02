#ifndef TEXTUREBASE_H_
#define TEXTUREBASE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::TextureBase

    (C) 2010 Christian Bleicher
*/
#include "pixelformat.h"
#include "rendertexture.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

enum TextureType
{
    /// 2D texture, used in combination with 2D texture coordinates (default)
    TEX_TYPE_2D,
    /// 3D cube map, used in combination with 3D texture coordinates
    TEX_TYPE_CUBE_MAP
};

class TextureBase
{
public:
    /// default constructor
    TextureBase();
    /// construct from rendertexture
    TextureBase(RenderTexture* const rt);
    /// construct cubemap
    TextureBase(void* faces[6U]);
    /// destructor
    virtual ~TextureBase();

    /// sets the pixel format for the texture surface
    void SetFormat(PixelFormat pf);
    /// gets the pixel format for the texture surface
    PixelFormat GetFormat() const;
    /// set the height of the texture
    void SetHeight(int h);
    /// get the height of the texture
    int GetHeight() const;
    /// set the width of the texture
    void SetWidth(int w);
    /// set the width of the texture
    int GetWidth() const;
    /// gets the number of mipmaps to be used for this texture
    int GetNumMipmaps() const;
    /// sets the number of mipmaps to be used for this texture
    void SetNumMipmaps(int num);

    /// set the hardware pixel buffer to use
    void SetBuffer(void* const buffer);
    /// return hardware pixel buffer for a surface
    const void* const GetBuffer() const;

    /// enable to swizzle texture read
    void SetSwizzleEnabled(bool enabled);
    /// returns if the pixelbuffer is swizzled
    bool GetSwizzleEnabled() const;

protected:
    TextureType type;
    PixelFormat format;
    int height;
    int width;
    int numMipmaps;
    bool swizzled;
    bool isRenderTarget;
    void* textureBuffer;
    void* cubeFaces[6U];
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
