#ifndef GLES2RENDERTEXTURE_H_
#define GLES2RENDERTEXTURE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::GLES2RenderTexture

    (C) 2012 Christian Bleicher
*/
#include "rendertarget.h"
#include <GLES2/gl2.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class GLES2RenderTexture : public graphics::RenderTarget
{
public:
    /// constructor
    GLES2RenderTexture();
    /// destructor
    ~GLES2RenderTexture();
    /// creates a rendertexture from the given parameters
    void Create(int width, int height, graphics::PixelFormat format, bool depth = false);
    /// does nothing no need for doublebuffering when rendering offline
    void SwapBuffers();
    /// get the type of the rendertarget
    unsigned int GetType() const;

    /// get fbo name
    GLuint GetFBO() const;
    /// get color attachment
    GLuint GetColourAttachment() const;

private:
    GLuint frameBufferObject;
    GLuint colourAttachment;
    GLuint depthAttachment;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
