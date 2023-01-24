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
    type(TEX_TYPE_2D),
    format(PF_UNKNOWN),
    height(0),
    width(0),
    numMipmaps(0),
    swizzled(true),
    isRenderTarget(false),
    textureBuffer(NULL),
    cubeFaces()
{

}

//------------------------------------------------------------------------------
/**
*/
TextureBase::TextureBase(RenderTexture* const rt) :
    type(TEX_TYPE_2D),
    format(rt->GetFormat()),
    height(rt->GetHeight()),
    width(rt->GetWidth()),
    numMipmaps(0),
    swizzled(false),
    isRenderTarget(true),
    textureBuffer(rt->GetBuffer()),
    cubeFaces()
{

}

//------------------------------------------------------------------------------
/**
*/
TextureBase::TextureBase(void* faces[6U]) :
    type(TEX_TYPE_CUBE_MAP),
    format(PF_UNKNOWN),
    height(0),
    width(0),
    numMipmaps(0),
    swizzled(true),
    isRenderTarget(false),
    textureBuffer(NULL)
{
    unsigned int i;
    for (i = 0U; i < 6U; ++i)
    {
        this->cubeFaces[i] = faces[i];
    }
}

//------------------------------------------------------------------------------
/**
*/
TextureBase::~TextureBase()
{
    if (!this->isRenderTarget)
    {
        if (TEX_TYPE_2D == this->type)
        {
            if (this->textureBuffer != NULL)
            {
                CE_FREE(this->textureBuffer);
            }
        }
        else if (TEX_TYPE_CUBE_MAP == this->type)
        {
            unsigned int i;
            for (i = 0U; i < 6U; ++i)
            {
                CE_FREE(this->cubeFaces[i]);
            }
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
