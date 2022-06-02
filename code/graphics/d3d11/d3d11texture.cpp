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
D3D11Texture::D3D11Texture(void* faces[6U]) :
    TextureBase(faces),
    shaderResourceView(NULL),
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
        desc.ArraySize = (graphics::TEX_TYPE_CUBE_MAP == this->type) ? 6U : 1U;
        desc.Format = D3D11Mappings::Get(this->format);
        desc.SampleDesc.Count = 1U;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0U;
        desc.MiscFlags = (graphics::TEX_TYPE_CUBE_MAP == this->type) ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0U;

        D3D11_SUBRESOURCE_DATA data[6U];
        ZeroMemory(&data, sizeof(data));
        if (graphics::TEX_TYPE_CUBE_MAP == type)
        {
            unsigned int i;
            for (i = 0U; i < 6U; ++i)
            {
                data[i].pSysMem = this->cubeFaces[i];
                data[i].SysMemPitch = D3D11Mappings::GetSysMemPitch((unsigned int)this->width, this->format);
            }
        }
        else
        {
            data[0].pSysMem = this->textureBuffer;
            data[0].SysMemPitch = D3D11Mappings::GetSysMemPitch((unsigned int)this->width, this->format);
        }

        HRESULT result = D3D11RenderSystem::Instance()->GetDevice()->CreateTexture2D(&desc, data, &this->texture);
        CE_ASSERT(SUCCEEDED(result), "D3D11Texture::CreateInternalResources(): failed to create 2d texture\n");

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MipLevels = desc.MipLevels;
        srvDesc.TextureCube.MostDetailedMip = 0U;

        result = D3D11RenderSystem::Instance()->GetDevice()->CreateShaderResourceView(this->texture, (graphics::TEX_TYPE_CUBE_MAP == this->type) ? &srvDesc : NULL, &this->shaderResourceView);
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
