//------------------------------------------------------------------------------
//  textureunitstate.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "textureunitstate.h"
#include "texturemanager.h"
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
    mipFilter(FO_POINT),
    lbo(LBO_REPLACE),
    lbt(LBT_COLOUR),
    mappingMode(TMM_TEXTURE_COORDS),
    projectionMappingMode(TPM_UV),
    texture(NULL),
    textureType(TEX_TYPE_2D)
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
TextureUnitState::SetTextureScale(float u, float v)
{
    this->uScale = u;
    this->vScale = v;
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
TextureUnitState::SetTextureFiltering(FilterOptions min, FilterOptions mag, FilterOptions mip)
{
    this->minFilter = min;
    this->magFilter = mag;
    this->mipFilter = mip;
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
TextureUnitState::SetTextureBlendOperation(LayerBlendType type, LayerBlendOperation op)
{
    this->lbt = type;
    this->lbo = op;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureUnitState::GetTextureBlendOperation(LayerBlendType& type, LayerBlendOperation& op) const
{
    type = this->lbt;
    op = this->lbo;
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
TextureUnitState::SetTextureName(const char* const name, TextureType type)
{
    this->textureName = name;
    this->textureType = type;
}

//------------------------------------------------------------------------------
/**
*/
const core::String&
TextureUnitState::GetTextureName() const
{
    return this->textureName;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureUnitState::SetTexture(Texture* tex)
{
    this->texture = tex;
}

//------------------------------------------------------------------------------
/**
*/
Texture* const
TextureUnitState::GetTexture() const
{
    return this->texture;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureUnitState::_Load()
{
    if (this->textureName.Size() > 0U)
    {
        this->texture = TextureManager::Instance()->Load(this->textureName.C_Str(), this->textureType);
    }
}

} // namespace graphics
} // namespace chrissly