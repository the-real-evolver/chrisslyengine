//------------------------------------------------------------------------------
//  gles2texture.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "gles2texture.h"
#include "gles2mappings.h"
#include "debug.h"

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
GLES2Texture::GLES2Texture() :
    textureName(0)
{

}

//------------------------------------------------------------------------------
/**
*/
GLES2Texture::GLES2Texture(graphics::RenderTexture* const rt) :
    TextureBase(rt),
    textureName(0)
{

}

//------------------------------------------------------------------------------
/**
*/
GLES2Texture::GLES2Texture(void* faces[6U]) :
    TextureBase(faces),
    textureName(0)
{
    this->textureBuffer = faces[0U]; // temporary workaround to avoid crash and at least render something
}

//------------------------------------------------------------------------------
/**
*/
GLES2Texture::~GLES2Texture()
{
    if (!this->isRenderTarget)
    {
        if (this->textureName != 0)
        {
            glDeleteTextures(1, &this->textureName);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2Texture::CreateInternalResources()
{
    if (!this->isRenderTarget)
    {
        glGenTextures(1, &this->textureName);
        glBindTexture(GL_TEXTURE_2D, this->textureName);

        if (GLES2Mappings::IsCompressed(this->format))
        {
            GLsizei imageSize = GLES2Mappings::GetImageSize(this->format, this->width, this->height);
            glCompressedTexImage2D(GL_TEXTURE_2D, 0, GLES2Mappings::Get(this->format), this->width, this->height, 0, imageSize, this->textureBuffer);
        }
        else
        {
            glPixelStorei(GL_UNPACK_ALIGNMENT, (graphics::PF_R8G8B8 == this->format) ? 1 : 4);
            glTexImage2D(GL_TEXTURE_2D, 0, GLES2Mappings::GetInternalFormat(this->format), this->width, this->height, 0,
                            GLES2Mappings::GetInternalFormat(this->format), GLES2Mappings::Get(this->format), this->textureBuffer);

            if (0 == this->numMipmaps)
            {
                glGenerateMipmap(GL_TEXTURE_2D);
                unsigned int mipmapWidth = this->width;
                unsigned int mipmapHeight = this->height;
                while (mipmapWidth > 1U && mipmapHeight > 1U)
                {
                    ++this->numMipmaps;
                    mipmapWidth = mipmapWidth >> 1U;
                    mipmapHeight = mipmapHeight >> 1U;
                }
            }
        }

        CE_LOG("GLES2Texture::CreateInternalResources(): numMipmaps: %i\n", this->numMipmaps);
    }
}

//------------------------------------------------------------------------------
/**
*/
GLuint
GLES2Texture::GetName() const
{
    return this->textureName;
}

} // namespace chrissly
