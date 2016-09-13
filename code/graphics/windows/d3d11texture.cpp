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
D3D11Texture::~D3D11Texture()
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

//------------------------------------------------------------------------------
/**
*/
void
D3D11Texture::CreateInternalResources()
{
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = (unsigned int)this->width;
    desc.Height = (unsigned int)this->height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = D3D11Mappings::Get(this->format);
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    ZeroMemory(&data, sizeof(data));
    data.pSysMem = this->textureBuffer;
    data.SysMemPitch = D3D11Mappings::GetSysMemPitch((unsigned int)this->width, this->format);

    HRESULT result = D3D11RenderSystem::Instance()->GetDevice()->CreateTexture2D(&desc, &data, &this->texture);
    CE_ASSERT(SUCCEEDED(result), "D3D11Texture::CreateInternalResources(): failed to create 2d texture\n");

    result = D3D11RenderSystem::Instance()->GetDevice()->CreateShaderResourceView(this->texture, NULL, &this->shaderResourceView);
    CE_ASSERT(SUCCEEDED(result), "D3D11Texture::CreateInternalResources(): failed to create resource view\n");
}

//------------------------------------------------------------------------------
/**
*/
ID3D11ShaderResourceView*
D3D11Texture::GetShaderResourceView() const
{
    return this->shaderResourceView;
}

} // namespace chrissly
