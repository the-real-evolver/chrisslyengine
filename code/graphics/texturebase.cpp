//------------------------------------------------------------------------------
//  texturebase.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "texturebase.h"
#include "memoryallocatorconfig.h"
#include <stdio.h>

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
TextureBase::TextureBase() :
    format(PF_UNKNOWN),
    height(0),
    width(0),
    numMipmaps(0),
    textureBuffer(NULL),
    swizzled(true)
{

}

//------------------------------------------------------------------------------
/**
*/
TextureBase::~TextureBase()
{
    if (this->textureBuffer != NULL)
    {
        CE_FREE(this->textureBuffer);
    }
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
TextureBase::SetBuffer(void* buffer)
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
