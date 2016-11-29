#ifndef GLES2RENDERTEXTURE_H_
#define GLES2RENDERTEXTURE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::GLES2RenderTexture

    (C) 2012 Christian Bleicher
*/
#include "rendertarget.h"

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
    void Create(int width, int height, graphics::PixelFormat format); 
    /// does nothing no need for doublebuffering when rendering offline
    void SwapBuffers();
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
