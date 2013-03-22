//------------------------------------------------------------------------------
//  pspmappings.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "pspmappings.h"
#include "debug.h"
#include <pspgu.h>

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
ScePspFMatrix4
PSPMappings::MakePSPMatrix(const core::Matrix4& mat)
{
    ScePspFMatrix4 m;

    m.x.x = mat[0][0]; m.y.x = mat[0][1]; m.z.x = mat[0][2]; m.w.x = mat[0][3];
    m.x.y = mat[1][0]; m.y.y = mat[1][1]; m.z.y = mat[1][2]; m.w.y = mat[1][3];
    m.x.z = mat[2][0]; m.y.z = mat[2][1]; m.z.z = mat[2][2]; m.w.z = mat[2][3];
    m.x.w = mat[3][0]; m.y.w = mat[3][1]; m.z.w = mat[3][2]; m.w.w = mat[3][3];

    return m;
}

//------------------------------------------------------------------------------
/**
*/
int
PSPMappings::Get(graphics::SceneBlendOperation op)
{
    switch (op)
    {
        case graphics::SBO_ADD:              return GU_ADD;
        case graphics::SBO_SUBTRACT:         return GU_SUBTRACT;
        case graphics::SBO_REVERSE_SUBTRACT: return GU_REVERSE_SUBTRACT;
        case graphics::SBO_MIN:              return GU_MIN;
        case graphics::SBO_MAX:              return GU_MAX;
        case graphics::SBO_ABS:              return GU_ABS;
        default: CE_ASSERT(false, "PSPMappings::Get(): illegal SceneBlendOperation '%i'\n", op);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
PSPMappings::Get(graphics::SceneBlendFactor sbf)
{
    switch (sbf)
    {
        case graphics::SBF_ONE:                     return GU_FIX;
        case graphics::SBF_ZERO:                    return GU_FIX;
        case graphics::SBF_DEST_COLOUR:             return GU_DST_COLOR;
        case graphics::SBF_SOURCE_COLOUR:           return GU_SRC_COLOR;
        case graphics::SBF_ONE_MINUS_DEST_COLOUR:   return GU_ONE_MINUS_DST_COLOR;
        case graphics::SBF_ONE_MINUS_SOURCE_COLOUR: return GU_ONE_MINUS_SRC_COLOR;
        case graphics::SBF_DEST_ALPHA:              return GU_DST_ALPHA;
        case graphics::SBF_SOURCE_ALPHA:            return GU_SRC_ALPHA;
        case graphics::SBF_ONE_MINUS_DEST_ALPHA:    return GU_ONE_MINUS_DST_ALPHA;
        case graphics::SBF_ONE_MINUS_SOURCE_ALPHA:  return GU_ONE_MINUS_SRC_ALPHA;
        case graphics::SBF_FIX:                     return GU_FIX; // only PSP
        default: CE_ASSERT(false, "PSPMappings::Get(): illegal SceneBlendFactor '%i'\n", sbf);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
PSPMappings::Get(graphics::LayerBlendType lbt)
{
    switch (lbt)
    {
        case graphics::LBT_COLOUR: return GU_TCC_RGB;
        case graphics::LBT_ALPHA:  return GU_TCC_RGBA;
        default: CE_ASSERT(false, "PSPMappings::Get(): illegal LayerBlendType '%i'\n", lbt);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
PSPMappings::Get(graphics::LayerBlendOperation lbo)
{
    switch (lbo)
    {
        case graphics::LBO_REPLACE:     return GU_TFX_REPLACE;
        case graphics::LBO_ADD:         return GU_TFX_ADD;
        case graphics::LBO_MODULATE:    return GU_TFX_MODULATE;
        case graphics::LBO_ALPHA_BLEND: return GU_TFX_BLEND;
        case graphics::LBO_DECAL:       return GU_TFX_DECAL;
        default: CE_ASSERT(false, "PSPMappings::Get(): illegal LayerBlendOperation '%i'\n", lbo);
    }

    return 0;
}
    
//------------------------------------------------------------------------------
/**
*/
void
PSPMappings::Get(graphics::FilterOptions minFilter, graphics::FilterOptions magFilter, graphics::FilterOptions mipFilter, int& min, int& mag)
{
    switch (mipFilter)
    {
        case graphics::FO_NONE:
            {
                switch (minFilter)
                {
                    case graphics::FO_NONE:   min = GU_NEAREST; break;
                    case graphics::FO_POINT:  min = GU_NEAREST; break;
                    case graphics::FO_LINEAR: min = GU_LINEAR;  break;
                    default: CE_ASSERT(false, "PSPMappings::Get(): illegal FilterOptions '%i'\n", minFilter);        
                }
            }
            break;

        case graphics::FO_POINT:
            {
                switch (minFilter)
                {
                    case graphics::FO_NONE:   min = GU_NEAREST_MIPMAP_NEAREST; break;
                    case graphics::FO_POINT:  min = GU_NEAREST_MIPMAP_NEAREST; break;
                    case graphics::FO_LINEAR: min = GU_LINEAR_MIPMAP_NEAREST;  break;
                    default: CE_ASSERT(false, "PSPMappings::Get(): illegal FilterOptions '%i'\n", minFilter);        
                }
            }
            break;

        case graphics::FO_LINEAR:
            {
                switch (minFilter)
                {
                    case graphics::FO_NONE:   min = GU_NEAREST_MIPMAP_LINEAR; break;
                    case graphics::FO_POINT:  min = GU_NEAREST_MIPMAP_LINEAR; break;
                    case graphics::FO_LINEAR: min = GU_LINEAR_MIPMAP_LINEAR;  break;
                    default: CE_ASSERT(false, "PSPMappings::Get(): illegal FilterOptions '%i'\n", minFilter);        
                }
            }
            break;

        default: CE_ASSERT(false, "PSPMappings::Get(): illegal FilterOptions '%i'\n", mipFilter);
    }

    switch (magFilter)
    {
        case graphics::FO_NONE:   mag = GU_NEAREST; break;
        case graphics::FO_POINT:  mag = GU_NEAREST; break;
        case graphics::FO_LINEAR: mag = GU_LINEAR;  break;
        default: CE_ASSERT(false, "PSPMappings::Get(): illegal FilterOptions '%i'\n", magFilter);
    }
}

//------------------------------------------------------------------------------
/**
*/
int
PSPMappings::Get(graphics::TextureUnitState::TextureAddressingMode tam)
{
    switch (tam)
    {
        case graphics::TextureUnitState::TAM_WRAP:   return GU_REPEAT;
        case graphics::TextureUnitState::TAM_CLAMP:  return GU_CLAMP;
        case graphics::TextureUnitState::TAM_BORDER: return GU_CLAMP;
        default: CE_ASSERT(false, "PSPMappings::Get(): illegal TextureAddressingMode '%i'\n", tam);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
PSPMappings::Get(graphics::TextureUnitState::TextureMappingMode tmm)
{
    switch (tmm)
    {
        case graphics::TextureUnitState::TMM_TEXTURE_COORDS:  return GU_TEXTURE_COORDS;
        case graphics::TextureUnitState::TMM_TEXTURE_MATRIX:  return GU_TEXTURE_MATRIX;
        case graphics::TextureUnitState::TMM_ENVIRONMENT_MAP: return GU_ENVIRONMENT_MAP;
        default: CE_ASSERT(false, "PSPMappings::Get(): illegal TextureMappingMode '%i'\n", tmm);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
PSPMappings::Get(graphics::TextureUnitState::TextureProjectionMappingMode tpm)
{
    switch (tpm)
    {
        case graphics::TextureUnitState::TPM_POSITION:          return GU_POSITION;
        case graphics::TextureUnitState::TPM_UV:                return GU_UV;
        case graphics::TextureUnitState::TPM_NORMALIZED_NORMAL: return GU_NORMALIZED_NORMAL;
        case graphics::TextureUnitState::TPM_NORMAL:            return GU_NORMAL;
        default: CE_ASSERT(false, "PSPMappings::Get(): illegal TextureProjectionMappingMode '%i'\n", tpm);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
PSPMappings::Get(graphics::PixelFormat pf)
{
    switch (pf)
    {
        case graphics::PF_R5G6B5:        return GU_PSM_5650;
        case graphics::PF_A1R5G5B5:      return GU_PSM_5551;
        case graphics::PF_A4R4G4B4:      return GU_PSM_4444;
        case graphics::PF_R8G8B8A8:      return GU_PSM_8888;
        case graphics::PF_COLOR_INDEX4:  return GU_PSM_T4;
        case graphics::PF_COLOR_INDEX8:  return GU_PSM_T8;
        case graphics::PF_COLOR_INDEX16: return GU_PSM_T16;
        case graphics::PF_COLOR_INDEX32: return GU_PSM_T32;
        case graphics::PF_DXT1:          return GU_PSM_DXT1;
        case graphics::PF_DXT3:          return GU_PSM_DXT3;
        case graphics::PF_DXT5:          return GU_PSM_DXT5;
        default: CE_ASSERT(false, "PSPMappings::Get(): illegal PixelFormat '%i'\n", pf);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
PSPMappings::Get(graphics::FrameBufferType fbt)
{
    int clearBits = 0;

    if (fbt & graphics::FBT_COLOUR)     clearBits |= GU_COLOR_BUFFER_BIT;
    if (fbt & graphics::FBT_DEPTH)      clearBits |= GU_DEPTH_BUFFER_BIT;
    if (fbt & graphics::FBT_STENCIL)    clearBits |= GU_STENCIL_BUFFER_BIT;
    if (fbt & graphics::FBT_FAST_CLEAR) clearBits |= GU_FAST_CLEAR_BIT;
 
    return clearBits;
}

//------------------------------------------------------------------------------
/**
*/
int
PSPMappings::Get(graphics::TrackVertexColourType tvct)
{
    int colorComponent = 0;

    if (tvct & graphics::TVC_NONE)     return GU_UNKNOWN_LIGHT_COMPONENT;
    if (tvct & graphics::TVC_AMBIENT)  colorComponent |= GU_AMBIENT;
    if (tvct & graphics::TVC_DIFFUSE)  colorComponent |= GU_DIFFUSE;
    if (tvct & graphics::TVC_SPECULAR) colorComponent |= GU_SPECULAR;

    return colorComponent;
}

//------------------------------------------------------------------------------
/**
*/
int
PSPMappings::Get(graphics::Light::LightTypes lt)
{
    if (graphics::Light::LT_DIRECTIONAL == lt) return GU_DIRECTIONAL;
    if (graphics::Light::LT_POINT == lt)       return GU_POINTLIGHT;
    if (graphics::Light::LT_SPOTLIGHT == lt)   return GU_SPOTLIGHT;

    CE_ASSERT(false, "PSPMappings::Get(): illegal LightTypes '%i'\n", lt);

    return 0;
}

} // namespace chrissly