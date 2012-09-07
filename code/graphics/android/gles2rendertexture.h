#ifndef GLES2RENDERTEXTURE_H_
#define GLES2RENDERTEXTURE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::GLES2RenderTexture

    (C) 2012 Christian Bleicher
*/
#include "rendertarget.h"

//------------------------------------------------------------------------------
namespace chrissly
{

class GLES2RenderTexture : public graphics::RenderTarget
{
public:
    /// constructor with buffer parameters
    GLES2RenderTexture();
    /// destructor
    ~GLES2RenderTexture();
    /// creates a rendertexture from the given parameters
    void Create(int width, int height, graphics::PixelFormat format); 
    /// tells the texture to update it's contents
    void Update();
    /// does nothing no need for doublebuffering when rendering offline
    void SwapBuffers();
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
