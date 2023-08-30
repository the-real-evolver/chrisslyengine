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
    textureName(0U)
{

}

//------------------------------------------------------------------------------
/**
*/
GLES2Texture::GLES2Texture(graphics::RenderTexture* const rt) :
    TextureBase(rt),
    textureName(0U)
{

}

//------------------------------------------------------------------------------
/**
*/
GLES2Texture::GLES2Texture(void* faces[6U]) :
    TextureBase(faces),
    textureName(0U)
{

}

//------------------------------------------------------------------------------
/**
*/
GLES2Texture::~GLES2Texture()
{
    if (!this->isRenderTarget)
    {
        if (this->textureName != 0U)
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
        GLenum texType = GLES2Mappings::Get(this->type);

        glGenTextures(1, &this->textureName);
        glBindTexture(texType, this->textureName);

        if (GLES2Mappings::IsCompressed(this->format))
        {
            GLsizei imageSize = GLES2Mappings::GetImageSize(this->format, this->width, this->height);
            if (graphics::TEX_TYPE_CUBE_MAP == this->type)
            {
                glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GLES2Mappings::Get(this->format), this->width, this->height, 0, imageSize, this->cubeFaces[0U]);
                glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GLES2Mappings::Get(this->format), this->width, this->height, 0, imageSize, this->cubeFaces[1U]);
                glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GLES2Mappings::Get(this->format), this->width, this->height, 0, imageSize, this->cubeFaces[2U]);
                glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GLES2Mappings::Get(this->format), this->width, this->height, 0, imageSize, this->cubeFaces[3U]);
                glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GLES2Mappings::Get(this->format), this->width, this->height, 0, imageSize, this->cubeFaces[4U]);
                glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GLES2Mappings::Get(this->format), this->width, this->height, 0, imageSize, this->cubeFaces[5U]);
            }
            else
            {
                glCompressedTexImage2D(GL_TEXTURE_2D, 0, GLES2Mappings::Get(this->format), this->width, this->height, 0, imageSize, this->textureBuffer);
            }
        }
        else
        {
            glPixelStorei(GL_UNPACK_ALIGNMENT, (graphics::PF_R8G8B8 == this->format) ? 1 : 4);
            if (graphics::TEX_TYPE_CUBE_MAP == this->type)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GLES2Mappings::GetInternalFormat(this->format), this->width, this->height, 0, GLES2Mappings::GetInternalFormat(this->format),GLES2Mappings::Get(this->format), this->cubeFaces[0U]);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GLES2Mappings::GetInternalFormat(this->format), this->width, this->height, 0, GLES2Mappings::GetInternalFormat(this->format),GLES2Mappings::Get(this->format), this->cubeFaces[1U]);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GLES2Mappings::GetInternalFormat(this->format), this->width, this->height, 0, GLES2Mappings::GetInternalFormat(this->format),GLES2Mappings::Get(this->format), this->cubeFaces[2U]);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GLES2Mappings::GetInternalFormat(this->format), this->width, this->height, 0, GLES2Mappings::GetInternalFormat(this->format),GLES2Mappings::Get(this->format), this->cubeFaces[3U]);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GLES2Mappings::GetInternalFormat(this->format), this->width, this->height, 0, GLES2Mappings::GetInternalFormat(this->format),GLES2Mappings::Get(this->format), this->cubeFaces[4U]);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GLES2Mappings::GetInternalFormat(this->format), this->width, this->height, 0, GLES2Mappings::GetInternalFormat(this->format),GLES2Mappings::Get(this->format), this->cubeFaces[5U]);
            }
            else
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GLES2Mappings::GetInternalFormat(this->format), this->width, this->height, 0, GLES2Mappings::GetInternalFormat(this->format), GLES2Mappings::Get(this->format), this->textureBuffer);
            }

            if (0 == this->numMipmaps)
            {
                glGenerateMipmap(texType);
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
