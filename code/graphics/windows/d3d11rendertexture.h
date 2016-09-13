#ifndef D3D11RENDERTEXTURE_H_
#define D3D11RENDERTEXTURE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::D3D11RenderTexture

    (C) 2016 Christian Bleicher
*/
#include "rendertarget.h"

//------------------------------------------------------------------------------
namespace chrissly
{

class D3D11RenderTexture : public graphics::RenderTarget
{
public:
    /// constructor
    D3D11RenderTexture();
    /// destructor
    ~D3D11RenderTexture();
    /// tells the texture to update it's contents
    void Update();
    /// does nothing no need for doublebuffering when rendering offline
    void SwapBuffers();
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
