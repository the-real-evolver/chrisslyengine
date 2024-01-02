//------------------------------------------------------------------------------
//  d3d11rendertexture.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "d3d11rendertexture.h"
#include "d3d11mappings.h"
#include "d3d11rendersystem.h"
#include "debug.h"

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
D3D11RenderTexture::D3D11RenderTexture() :
    renderTargetView(NULL),
    shaderResourceView(NULL),
    texture(NULL),
    depthStencilBuffer(NULL),
    depthStencilView(NULL)
{

}

//------------------------------------------------------------------------------
/**
*/
D3D11RenderTexture::~D3D11RenderTexture()
{
    this->RemoveAllViewports();

    if (this->shaderResourceView != NULL)
    {
        this->shaderResourceView->Release();
    }
    if (this->renderTargetView != NULL)
    {
        this->renderTargetView->Release();
    }
    if (this->texture != NULL)
    {
        this->texture->Release();
    }
    if (this->depthStencilBuffer != NULL)
    {
        this->depthStencilBuffer->Release();
    }
    if (this->depthStencilView != NULL)
    {
        this->depthStencilView->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderTexture::Create(int w, int h, graphics::PixelFormat fmt, bool depth)
{
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = (UINT)w;
    desc.Height = (UINT)h;
    desc.MipLevels = 1U;
    desc.ArraySize = 1U;
    desc.Format = D3D11Mappings::Get(fmt);
    desc.SampleDesc.Count = 1U;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0U;
    desc.MiscFlags = 0U;
    HRESULT result = D3D11RenderSystem::Instance()->GetDevice()->CreateTexture2D(&desc, NULL, &this->texture);
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderTexture::Create(): failed to create 2d texture\n");

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = desc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    result = D3D11RenderSystem::Instance()->GetDevice()->CreateRenderTargetView(this->texture, &rtvDesc, &this->renderTargetView);
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderTexture::Create(): failed to create rendertarget view\n");

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    result = D3D11RenderSystem::Instance()->GetDevice()->CreateShaderResourceView(this->texture, &srvDesc, &this->shaderResourceView);
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderTexture::Create(): failed to create resource view\n");

    if (depth)
    {
        D3D11RenderSystem::Instance()->CreateDepthBuffer((UINT)w, (UINT)h, false, &this->depthStencilBuffer, &this->depthStencilView);
    }

    this->width = w;
    this->height = h;
    this->format = fmt;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderTexture::SwapBuffers()
{
    // do nothing no need for doublebuffering
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
D3D11RenderTexture::GetType() const
{
    return 'DXRT';
}

//------------------------------------------------------------------------------
/**
*/
ID3D11RenderTargetView* const
D3D11RenderTexture::GetRenderTargetView() const
{
    return this->renderTargetView;
}

//------------------------------------------------------------------------------
/**
*/
ID3D11DepthStencilView* const
D3D11RenderTexture::GetDepthStencilView() const
{
    return this->depthStencilView;
}

//------------------------------------------------------------------------------
/**
*/
ID3D11ShaderResourceView* const
D3D11RenderTexture::GetShaderResourceView() const
{
    return this->shaderResourceView;
}

} // namespace chrissly