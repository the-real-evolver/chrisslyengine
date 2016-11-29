#ifndef PSPRENDERTEXTURE_H_
#define PSPRENDERTEXTURE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::PSPRenderTexture

    (C) 2011 Christian Bleicher
*/
#include "rendertarget.h"

//------------------------------------------------------------------------------
namespace chrissly
{

class PSPRenderTexture : public graphics::RenderTarget
{
public:
    /// constructor
    PSPRenderTexture();
    /// destructor
    ~PSPRenderTexture();
    /// creates a rendertexture from the given parameters
    void Create(int width, int height, graphics::PixelFormat format);
    /// does nothing no need for doublebuffering when rendering offline
    void SwapBuffers();
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
