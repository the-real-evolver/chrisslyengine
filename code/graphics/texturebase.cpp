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

//------------------------------------------------------------------------------
/**
*/
void
TextureBase::CreateInternalResourcesImpl()
{
    // overwrite in platform specific subclass
}
    
} // namespace graphics
} // namespace chrissly
