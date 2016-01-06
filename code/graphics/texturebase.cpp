//------------------------------------------------------------------------------
//  texturebase.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "texturebase.h"

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
TextureBase::TextureBase() :
    format(PF_R5G6B5),
    height(0),
    width(0),
    numMipmaps(0),
    textureBuffer(0),
    swizzled(true)
{

}

//------------------------------------------------------------------------------
/**
*/
TextureBase::~TextureBase()
{

}

//------------------------------------------------------------------------------
/**
*/
void
TextureBase::SetFormat(PixelFormat pf)
{
    this->format = pf;
}

//------------------------------------------------------------------------------
/**
*/
PixelFormat
TextureBase::GetFormat() const
{
    return this->format;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureBase::SetHeight(int h)
{
    this->height = h;
}

//------------------------------------------------------------------------------
/**
*/
int
TextureBase::GetHeight() const
{
    return this->height;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureBase::SetWidth(int w)
{
    this->width = w;
}

//------------------------------------------------------------------------------
/**
*/
int
TextureBase::GetWidth() const
{
    return this->width;
}

//------------------------------------------------------------------------------
/**
*/
int
TextureBase::GetNumMipmaps() const
{
    return this->numMipmaps;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureBase::SetNumMipmaps(int num)
{
    this->numMipmaps = num;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureBase::SetBuffer(const void* buffer)
{
    this->textureBuffer = buffer;
}

//------------------------------------------------------------------------------
/**
*/
const void*
TextureBase::GetBuffer() const
{
    return this->textureBuffer;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureBase::SetSwizzleEnabled(bool enabled)
{
    this->swizzled = enabled;
}
    
//------------------------------------------------------------------------------
/**
*/
bool
TextureBase::GetSwizzleEnabled() const
{
    return this->swizzled;
}

} // namespace graphics
} // namespace chrissly
