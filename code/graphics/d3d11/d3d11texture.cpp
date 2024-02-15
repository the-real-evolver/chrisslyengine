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
        desc.MipLevels = (UINT)this->numMipmaps + 1U;
        desc.ArraySize = (graphics::TEX_TYPE_CUBE_MAP == this->type) ? 6U : 1U;
        desc.Format = D3D11Mappings::Get(this->format);
        desc.SampleDesc.Count = 1U;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0U;
        desc.MiscFlags = (graphics::TEX_TYPE_CUBE_MAP == this->type) ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0U;

        D3D11_SUBRESOURCE_DATA data[16U]; // maximum texture dimension in d3d11 is 16384 so the maximum number of miplevels is 15
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
            void* mipmapbuffer = this->textureBuffer;
            unsigned int mipmapWidth = this->width, mipmapHeight = this->height;
            unsigned int i;
            for (i = 0U; i < this->numMipmaps + 1U; ++i)
            {
                data[i].pSysMem = mipmapbuffer;
                data[i].SysMemPitch = D3D11Mappings::GetSysMemPitch(mipmapWidth, this->format);
                mipmapbuffer = (void*)((uintptr_t)mipmapbuffer + D3D11Mappings::GetMemorySize(mipmapWidth, mipmapHeight, this->format));
                mipmapWidth = mipmapWidth >> 1U;
                mipmapHeight = mipmapHeight >> 1U;
            }
        }
        HRESULT result = D3D11RenderSystem::Instance()->GetDevice()->CreateTexture2D(&desc, data, &this->texture);
        CE_ASSERT(SUCCEEDED(result), "D3D11Texture::CreateInternalResources(): failed to create 2d texture\n");

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = desc.Format;
        if (graphics::TEX_TYPE_CUBE_MAP == type)
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            srvDesc.TextureCube.MipLevels = desc.MipLevels;
            srvDesc.TextureCube.MostDetailedMip = 0U;
        }
        else
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = desc.MipLevels;
            srvDesc.Texture2D.MostDetailedMip = 0U;
        }
        result = D3D11RenderSystem::Instance()->GetDevice()->CreateShaderResourceView(this->texture, &srvDesc, &this->shaderResourceView);
        CE_ASSERT(SUCCEEDED(result), "D3D11Texture::CreateInternalResources(): failed to create resource view\n");
    }
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
