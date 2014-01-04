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
GLES2Texture::GLES2Texture() : textureName(0)
{

}

//------------------------------------------------------------------------------
/**
*/
GLES2Texture::~GLES2Texture()
{
    if (this->textureName != 0)
    {
        glDeleteTextures(1, &this->textureName);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2Texture::CreateInternalResourcesImpl()
{
    glGenTextures(1, &this->textureName);
    glBindTexture(GL_TEXTURE_2D, this->textureName);

    if (GLES2Mappings::IsCompressed(this->format))
    {
        GLuint imageSize = (this->width * this->height) >> 1; // ETC1 provides 6x compression of 24-bit RGB
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GLES2Mappings::Get(this->format), this->width, this->height, 0, imageSize, this->textureBuffer);
    }
    else
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, (graphics::PF_R8G8B8 == this->format) ? 1 : 4);
        glTexImage2D(GL_TEXTURE_2D, 0, GLES2Mappings::GetInternalFormat(this->format), this->width, this->height, 0,
                        GLES2Mappings::GetInternalFormat(this->format), GLES2Mappings::Get(this->format), this->textureBuffer);
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    this->numMipmaps = 0;
    int mipmapWidth = this->width;
    int mipmapHeight = this->height;
    while (mipmapWidth > 1 && mipmapHeight > 1)
    {
        this->numMipmaps++;
        mipmapWidth = mipmapWidth >> 1;
        mipmapHeight = mipmapHeight >> 1;
    }

    CE_LOG("GLES2Texture::CreateInternalResourcesImpl(): numMipmaps: %i\n", this->numMipmaps);
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
