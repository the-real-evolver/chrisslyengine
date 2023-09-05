//------------------------------------------------------------------------------
//  gles2rendertexture.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "gles2rendertexture.h"
#include "gles2mappings.h"
#include "debug.h"

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
GLES2RenderTexture::GLES2RenderTexture() :
    frameBufferObject(0U),
    colourAttachment(0U),
    depthAttachment(0U)
{

}

//------------------------------------------------------------------------------
/**
*/
GLES2RenderTexture::~GLES2RenderTexture()
{
    this->RemoveAllViewports();

    if (this->frameBufferObject != 0U)
    {
        glDeleteFramebuffers(1, &this->frameBufferObject);
    }
    if (this->colourAttachment != 0U)
    {
        glDeleteTextures(1, &this->colourAttachment);
    }
    if (this->depthAttachment != 0U)
    {
        glDeleteTextures(1, &this->depthAttachment);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderTexture::Create(int width, int height, graphics::PixelFormat format, bool depth)
{
    glGenFramebuffers(1, &this->frameBufferObject);
    glBindFramebuffer(GL_FRAMEBUFFER, this->frameBufferObject);

    // create color attachment texture
    glGenTextures(1, &this->colourAttachment);
    glBindTexture(GL_TEXTURE_2D, this->colourAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GLES2Mappings::GetInternalFormat(format), width, height, 0, GLES2Mappings::GetInternalFormat(format), GLES2Mappings::Get(format), NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colourAttachment, 0);

    if (depth)
    {
        // create depth attachment texture
        glGenTextures(1, &this->depthAttachment);
        glBindTexture(GL_TEXTURE_2D, this->depthAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthAttachment, 0);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        CE_ASSERT(false, "GLES2RenderTexture::Create(): framebuffer is not complete");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0U);

    this->width = width;
    this->height = height;
    this->format = format;
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderTexture::SwapBuffers()
{
    // do nothing no need for doublebuffering
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
GLES2RenderTexture::GetType() const
{
    return 'GLRT';
}

//------------------------------------------------------------------------------
/**
*/
GLuint
GLES2RenderTexture::GetFBO() const
{
    return this->frameBufferObject;
}

//------------------------------------------------------------------------------
/**
*/
GLuint
GLES2RenderTexture::GetColourAttachment() const
{
    return this->colourAttachment;
}

} // namespace chrissly