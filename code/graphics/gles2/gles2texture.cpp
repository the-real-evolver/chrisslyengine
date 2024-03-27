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
    textureName(rt->GetColourAttachment())
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

        GLenum internalFormat = GLES2Mappings::GetInternalFormat(this->format);
        GLenum texelDataType = GLES2Mappings::Get(this->format);
        unsigned int mipWidth = this->width, mipHeight = this->height;
        uintptr_t offset = 0U;
        GLint mipLevel;
        for (mipLevel = 0U; mipLevel <= this->numMipmaps; ++mipLevel)
        {
            GLsizei imageSize = GLES2Mappings::GetImageSize(this->format, mipWidth, mipHeight);
            if (GLES2Mappings::IsCompressed(this->format))
            {
                if (graphics::TEX_TYPE_CUBE_MAP == this->type)
                {
                    glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mipLevel, texelDataType, (GLsizei)mipWidth, (GLsizei)mipHeight, 0, imageSize, (void*)((uintptr_t)this->cubeFaces[0U] + offset));
                    glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mipLevel, texelDataType, (GLsizei)mipWidth, (GLsizei)mipHeight, 0, imageSize, (void*)((uintptr_t)this->cubeFaces[1U] + offset));
                    glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mipLevel, texelDataType, (GLsizei)mipWidth, (GLsizei)mipHeight, 0, imageSize, (void*)((uintptr_t)this->cubeFaces[2U] + offset));
                    glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mipLevel, texelDataType, (GLsizei)mipWidth, (GLsizei)mipHeight, 0, imageSize, (void*)((uintptr_t)this->cubeFaces[3U] + offset));
                    glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mipLevel, texelDataType, (GLsizei)mipWidth, (GLsizei)mipHeight, 0, imageSize, (void*)((uintptr_t)this->cubeFaces[4U] + offset));
                    glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mipLevel, texelDataType, (GLsizei)mipWidth, (GLsizei)mipHeight, 0, imageSize, (void*)((uintptr_t)this->cubeFaces[5U] + offset));
                }
                else
                {
                    glCompressedTexImage2D(GL_TEXTURE_2D, mipLevel, texelDataType, (GLsizei)mipWidth, (GLsizei)mipHeight, 0, imageSize, (void*)((uintptr_t)this->textureBuffer + offset));
                }
            }
            else
            {
                glPixelStorei(GL_UNPACK_ALIGNMENT, (graphics::PF_R8G8B8 == this->format) ? 1 : 4);
                if (graphics::TEX_TYPE_CUBE_MAP == this->type)
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mipLevel, internalFormat, (GLsizei)mipWidth, (GLsizei)mipHeight, 0, internalFormat, texelDataType, (void*)((uintptr_t)this->cubeFaces[0U] + offset));
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mipLevel, internalFormat, (GLsizei)mipWidth, (GLsizei)mipHeight, 0, internalFormat, texelDataType, (void*)((uintptr_t)this->cubeFaces[1U] + offset));
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mipLevel, internalFormat, (GLsizei)mipWidth, (GLsizei)mipHeight, 0, internalFormat, texelDataType, (void*)((uintptr_t)this->cubeFaces[2U] + offset));
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mipLevel, internalFormat, (GLsizei)mipWidth, (GLsizei)mipHeight, 0, internalFormat, texelDataType, (void*)((uintptr_t)this->cubeFaces[3U] + offset));
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mipLevel, internalFormat, (GLsizei)mipWidth, (GLsizei)mipHeight, 0, internalFormat, texelDataType, (void*)((uintptr_t)this->cubeFaces[4U] + offset));
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mipLevel, internalFormat, (GLsizei)mipWidth, (GLsizei)mipHeight, 0, internalFormat, texelDataType, (void*)((uintptr_t)this->cubeFaces[5U] + offset));
                }
                else
                {
                    glTexImage2D(GL_TEXTURE_2D, mipLevel, internalFormat, (GLsizei)mipWidth, (GLsizei)mipHeight, 0, internalFormat, texelDataType, (void*)((uintptr_t)this->textureBuffer + offset));
                }
            }

            offset += imageSize;
            mipWidth = mipWidth >> 1U;
            mipHeight = mipHeight >> 1U;
        }

        if (this->numMipmaps > 0 && mipWidth > 1U && mipHeight > 1U)
        {
            // some drivers/devices will render black if the mipmap chain is incomplete and GL_TEXTURE_MIN_FILTER is not GL_NEAREST or GL_LINEAR
            CE_LOG("GLES2Texture::CreateInternalResources(): incomplete mipmap chain: smallest miplevel has %ux%u px\n", mipWidth, mipHeight);
        }

        if (0 == this->numMipmaps && !GLES2Mappings::IsCompressed(this->format))
        {
            glGenerateMipmap(texType);
            mipWidth = this->width;
            mipHeight = this->height;
            while (mipWidth > 1U && mipHeight > 1U)
            {
                ++this->numMipmaps;
                mipWidth = mipWidth >> 1U;
                mipHeight = mipHeight >> 1U;
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
