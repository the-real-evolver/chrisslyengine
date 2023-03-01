#ifndef D3D11RENDERTEXTURE_H_
#define D3D11RENDERTEXTURE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::D3D11RenderTexture

    (C) 2016 Christian Bleicher
*/
#include "rendertarget.h"
#include <d3d11.h>

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
    /// creates a rendertexture from the given parameters
    void Create(int w, int h, graphics::PixelFormat fmt, bool depth = false);
    /// does nothing no need for doublebuffering when rendering offline
    void SwapBuffers();
    /// get the type of the rendertarget
    unsigned int GetType() const;

    /// gets a pointer to the shader resource view
    ID3D11ShaderResourceView* const GetShaderResourceView() const;
    /// gets a pointer to the rendertarget view
    ID3D11RenderTargetView* const GetRenderTargetView() const;
    /// gets a pointer to the depth stencil view
    ID3D11DepthStencilView* const GetDepthStencilView() const;

private:
    ID3D11RenderTargetView* renderTargetView;
    ID3D11ShaderResourceView* shaderResourceView;
    ID3D11Texture2D* texture;
    ID3D11Texture2D* depthStencilBuffer;
    ID3D11DepthStencilView* depthStencilView;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
