//------------------------------------------------------------------------------
//  d3d11texture.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "d3d11texture.h"
#include "d3d11mappings.h"
#include "d3d11rendersystem.h"
#include "debug.h"

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
D3D11Texture::D3D11Texture() :
    shaderResourceView(NULL),
    texture(NULL)
{

}

//------------------------------------------------------------------------------
/**
*/
D3D11Texture::D3D11Texture(graphics::RenderTexture* const rt) :
    TextureBase(rt),
    shaderResourceView(rt->GetShaderResourceView()),
    texture(NULL)
{

}

//------------------------------------------------------------------------------
/**
*/
D3D11Texture::~D3D11Texture()
{
    if (!this->isRenderTarget)
    {
        if (this->shaderResourceView != NULL)
        {
            this->shaderResourceView->Release();
        }
        if (this->texture != NULL)
        {
            this->texture->Release();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11Texture::CreateInternalResources()
{
    if (!this->isRenderTarget)
    {
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = (UINT)this->width;
        desc.Height = (UINT)this->height;
        desc.MipLevels = 1U;
        desc.ArraySize = 1U;
        desc.Format = D3D11Mappings::Get(this->format);
        desc.SampleDesc.Count = 1U;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0U;
        desc.MiscFlags = 0U;

        D3D11_SUBRESOURCE_DATA data;
        ZeroMemory(&data, sizeof(data));
        data.pSysMem = this->textureBuffer;
        data.SysMemPitch = D3D11Mappings::GetSysMemPitch((unsigned int)this->width, this->format);

        HRESULT result = D3D11RenderSystem::Instance()->GetDevice()->CreateTexture2D(&desc, &data, &this->texture);
        CE_ASSERT(SUCCEEDED(result), "D3D11Texture::CreateInternalResources(): failed to create 2d texture\n");

        result = D3D11RenderSystem::Instance()->GetDevice()->CreateShaderResourceView(this->texture, NULL, &this->shaderResourceView);
        CE_ASSERT(SUCCEEDED(result), "D3D11Texture::CreateInternalResources(): failed to create resource view\n");
    }
}

//------------------------------------------------------------------------------
/**
*/
ID3D11ShaderResourceView* const
D3D11Texture::GetShaderResourceView() const
{
    return this->shaderResourceView;
}

} // namespace chrissly
