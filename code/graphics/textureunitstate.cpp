//------------------------------------------------------------------------------
//  textureunitstate.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "textureunitstate.h"
#include <stddef.h>

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
TextureUnitState::TextureUnitState() :
    uScale(1.0f),
    vScale(1.0f),
    uMod(0.0f),
    vMod(0.0f),
    minFilter(FO_LINEAR),
    magFilter(FO_LINEAR),
    mipFilter(FO_NONE),
    lbo(LBO_REPLACE),
    lbt(LBT_COLOUR),
    mappingMode(TMM_TEXTURE_COORDS),
    projectionMappingMode(TPM_UV),
    texture(NULL)
{
    this->addressMode.u = TAM_WRAP;
    this->addressMode.v = TAM_WRAP;
}

//------------------------------------------------------------------------------
/**
*/
TextureUnitState::~TextureUnitState()
{
    
}

//------------------------------------------------------------------------------
/**
*/
void
TextureUnitState::SetTextureScale(float uScale, float vScale)
{
    this->uScale = uScale;
    this->vScale = vScale;
}

//------------------------------------------------------------------------------
/**
*/
float
TextureUnitState::GetTextureUScale() const
{
    return this->uScale;
}

//------------------------------------------------------------------------------
/**
*/
float
TextureUnitState::GetTextureVScale() const
{
    return this->vScale;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureUnitState::SetTextureScroll(float u, float v)
{
    this->uMod = u;
    this->vMod = v;
}

//------------------------------------------------------------------------------
/**
*/
float
TextureUnitState::GetTextureUScroll() const
{
    return this->uMod;
}

//------------------------------------------------------------------------------
/**
*/
float
TextureUnitState::GetTextureVScroll() const
{
    return this->vMod;
}
    
//------------------------------------------------------------------------------
/**
*/
void
TextureUnitState::SetTextureFiltering(FilterOptions minFilter, FilterOptions magFilter, FilterOptions mipFilter)
{
    this->minFilter = minFilter;
    this->magFilter = magFilter;
    this->mipFilter = mipFilter;
}

//------------------------------------------------------------------------------
/**
*/
FilterOptions
TextureUnitState::GetTextureFiltering(FilterType ftype) const
{
    switch (ftype)
    {
        case FT_MIN:
            return this->minFilter;
            break;
        case FT_MAG:
            return this->magFilter;
            break;
        case FT_MIP:
            return this->mipFilter;
            break;
    }
    
    return FO_NONE;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureUnitState::SetTextureBlendOperation(LayerBlendType lbt, LayerBlendOperation lbo)
{
    this->lbo = lbo;
    this->lbt = lbt;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureUnitState::GetTextureBlendOperation(LayerBlendType& lbt, LayerBlendOperation& lbo) const
{
    lbo = this->lbo;
    lbt = this->lbt;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureUnitState::SetTextureAddressingMode(TextureAddressingMode u, TextureAddressingMode v)
{
    this->addressMode.u = u;
    this->addressMode.v = v;
}

//------------------------------------------------------------------------------
/**
*/
const TextureUnitState::UVWAddressingMode&
TextureUnitState::GetTextureAddressingMode() const
{
    return this->addressMode;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureUnitState::SetTextureMappingMode(TextureMappingMode mode)
{
    this->mappingMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
TextureUnitState::TextureMappingMode
TextureUnitState::GetTextureMappingMode() const
{
    return this->mappingMode;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureUnitState::SetTextureProjectionMappingMode(TextureProjectionMappingMode mode)
{
    this->projectionMappingMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
TextureUnitState::TextureProjectionMappingMode
TextureUnitState::GetTextureProjectionMappingMode() const
{
    return this->projectionMappingMode;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureUnitState::SetTexture(Texture* texture)
{
    this->texture = texture;
}

//------------------------------------------------------------------------------
/**
*/
Texture*
TextureUnitState::GetTexture() const
{
    return this->texture;
}

} // namespace graphics
} // namespace chrissly