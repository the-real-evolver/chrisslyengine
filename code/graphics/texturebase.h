#ifndef TEXTUREBASE_H_
#define TEXTUREBASE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::TextureBase

    (C) 2010 Christian Bleicher
*/
#include "pixelformat.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class TextureBase
{
public:
    /// default constructor
    TextureBase();
    /// destructor
    ~TextureBase();
    
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

    /// set the hardware pixel buffer to use
    void SetBuffer(const void* buffer);
    /// return hardware pixel buffer for a surface
    const void* GetBuffer() const;
    
    /// enable to swizzle texture read
    void SetSwizzleEnabled(bool enabled);
    /// returns if the pixelbuffer is swizzled
    bool GetSwizzleEnabled() const;
    
    /// implementation of creating internal texture resources 
    virtual void CreateInternalResourcesImpl();

protected:
    PixelFormat format;
    int height;
    int width;
    const void* textureBuffer;
    bool swizzled;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
