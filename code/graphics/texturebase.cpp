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
    swizzled(true),
    isRenderTarget(false)
{

}

//------------------------------------------------------------------------------
/**
*/
TextureBase::TextureBase(RenderTexture* const rt) :
    format(rt->GetFormat()),
    height(rt->GetHeight()),
    width(rt->GetWidth()),
    numMipmaps(0),
    textureBuffer(rt->GetBuffer()),
    swizzled(false),
    isRenderTarget(true)
{

}

//------------------------------------------------------------------------------
/**
*/
TextureBase::~TextureBase()
{
    if (!this->isRenderTarget)
    {
        if (this->textureBuffer != NULL)
        {
            CE_FREE(this->textureBuffer);
        }
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
TextureBase::SetBuffer(void* const buffer)
{
    this->textureBuffer = buffer;
}

//------------------------------------------------------------------------------
/**
*/
const void* const
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
