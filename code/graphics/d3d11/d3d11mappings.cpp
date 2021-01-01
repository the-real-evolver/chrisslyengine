//------------------------------------------------------------------------------
//  d3d11mappings.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "d3d11mappings.h"
#include "debug.h"

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
D3D11_BLEND_OP
D3D11Mappings::Get(graphics::SceneBlendOperation op)
{
    switch (op)
    {
        case graphics::SBO_ADD:                 return D3D11_BLEND_OP_ADD;
        case graphics::SBO_SUBTRACT:            return D3D11_BLEND_OP_SUBTRACT;
        case graphics::SBO_REVERSE_SUBTRACT:    return D3D11_BLEND_OP_REV_SUBTRACT;
        case graphics::SBO_MIN:                 return D3D11_BLEND_OP_MIN;
        case graphics::SBO_MAX:                 return D3D11_BLEND_OP_MAX;
        case graphics::SBO_ABS:
        default: CE_ASSERT(false, "D3D11Mappings::Get(): illegal SceneBlendOperation '%i'\n", op);
    }

    return D3D11_BLEND_OP();
}

//------------------------------------------------------------------------------
/**
*/
D3D11_BLEND
D3D11Mappings::Get(graphics::SceneBlendFactor sbf)
{
    switch (sbf)
    {
        case graphics::SBF_ONE:                     return D3D11_BLEND_ONE;
        case graphics::SBF_ZERO:                    return D3D11_BLEND_ZERO;
        case graphics::SBF_DEST_COLOUR:             return D3D11_BLEND_DEST_COLOR;
        case graphics::SBF_SOURCE_COLOUR:           return D3D11_BLEND_SRC_COLOR;
        case graphics::SBF_ONE_MINUS_DEST_COLOUR:   return D3D11_BLEND_INV_DEST_COLOR;
        case graphics::SBF_ONE_MINUS_SOURCE_COLOUR: return D3D11_BLEND_INV_SRC_COLOR;
        case graphics::SBF_DEST_ALPHA:              return D3D11_BLEND_DEST_ALPHA;
        case graphics::SBF_SOURCE_ALPHA:            return D3D11_BLEND_SRC_ALPHA;
        case graphics::SBF_ONE_MINUS_DEST_ALPHA:    return D3D11_BLEND_INV_DEST_ALPHA;
        case graphics::SBF_ONE_MINUS_SOURCE_ALPHA:  return D3D11_BLEND_INV_SRC_ALPHA;
        case graphics::SBF_FIX:
        default: CE_ASSERT(false, "D3D11Mappings::Get(): illegal SceneBlendFactor '%i'\n", sbf);
    }

    return D3D11_BLEND();
}

//------------------------------------------------------------------------------
/**
*/
D3D11_FILTER
D3D11Mappings::Get(graphics::FilterOptions minFilter, graphics::FilterOptions magFilter, graphics::FilterOptions mipFilter)
{
    switch (minFilter)
    {
        case graphics::FO_NONE:
        case graphics::FO_POINT:
        {
            switch (magFilter)
            {
                case graphics::FO_NONE:
                case graphics::FO_POINT:
                    switch (mipFilter)
                    {
                        case graphics::FO_NONE:
                        case graphics::FO_POINT:    return D3D11_FILTER_MIN_MAG_MIP_POINT;
                        case graphics::FO_LINEAR:   return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
                    }
                    break;
                case graphics::FO_LINEAR:
                    switch (mipFilter)
                    {
                        case graphics::FO_NONE:
                        case graphics::FO_POINT:    return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
                        case graphics::FO_LINEAR:   return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
                    }
                    break;
            }
        }
        break;

        case graphics::FO_LINEAR:
        {
            switch (magFilter)
            {
                case graphics::FO_NONE:
                case graphics::FO_POINT:
                    switch (mipFilter)
                    {
                        case graphics::FO_NONE:
                        case graphics::FO_POINT:    return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
                        case graphics::FO_LINEAR:   return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
                    }
                    break;
                case graphics::FO_LINEAR:
                    switch (mipFilter)
                    {
                        case graphics::FO_NONE:
                        case graphics::FO_POINT:    return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
                        case graphics::FO_LINEAR:   return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
                    }
                    break;
            }
        }
        break;
    }

    CE_ASSERT(false, "D3D11Mappings::Get(): illegal FilterOptions '%i' '%i' '%i'\n", minFilter, magFilter, mipFilter);

    return D3D11_FILTER();
}

//------------------------------------------------------------------------------
/**
*/
D3D11_TEXTURE_ADDRESS_MODE
D3D11Mappings::Get(graphics::TextureUnitState::TextureAddressingMode tam)
{
    switch (tam)
    {
        case graphics::TextureUnitState::TAM_WRAP:      return D3D11_TEXTURE_ADDRESS_WRAP;
        case graphics::TextureUnitState::TAM_MIRROR:    return D3D11_TEXTURE_ADDRESS_MIRROR;
        case graphics::TextureUnitState::TAM_CLAMP:     return D3D11_TEXTURE_ADDRESS_CLAMP;
        case graphics::TextureUnitState::TAM_BORDER:    return D3D11_TEXTURE_ADDRESS_BORDER;
        default: CE_ASSERT(false, "D3D11Mappings::Get(): illegal TextureAddressingMode '%i'\n", tam);
    }

    return D3D11_TEXTURE_ADDRESS_MODE();
}

//------------------------------------------------------------------------------
/**
*/
DXGI_FORMAT
D3D11Mappings::Get(graphics::PixelFormat pf)
{
    switch (pf)
    {
        case graphics::PF_R8G8B8A8:      return DXGI_FORMAT_R8G8B8A8_UNORM;
        case graphics::PF_DXT1:          return DXGI_FORMAT_BC1_UNORM;
        case graphics::PF_DXT3:          return DXGI_FORMAT_BC2_UNORM;
        case graphics::PF_DXT5:          return DXGI_FORMAT_BC3_UNORM;
        case graphics::PF_FLOAT32_RGBA:  return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case graphics::PF_R5G6B5:
        case graphics::PF_A1R5G5B5:
        case graphics::PF_A4R4G4B4:
        case graphics::PF_COLOR_INDEX4:
        case graphics::PF_COLOR_INDEX8:
        case graphics::PF_COLOR_INDEX16:
        case graphics::PF_COLOR_INDEX32:
        default: CE_ASSERT(false, "D3D11Mappings::Get(): illegal or unsupported PixelFormat '%i'\n", pf);
    }

    return DXGI_FORMAT_UNKNOWN;
}

//------------------------------------------------------------------------------
/**
*/
UINT
D3D11Mappings::Get(unsigned int fbt)
{
    UINT clearFlags = 0U;

    if (fbt & graphics::FBT_DEPTH)      {clearFlags |= D3D11_CLEAR_DEPTH;}
    if (fbt & graphics::FBT_STENCIL)    {clearFlags |= D3D11_CLEAR_STENCIL;}

    return clearFlags;
}

//------------------------------------------------------------------------------
/**
*/
UINT
D3D11Mappings::GetSysMemPitch(unsigned int width, graphics::PixelFormat pf)
{
    switch (pf)
    {
        case graphics::PF_R8G8B8A8:      return width << 2U;
        case graphics::PF_DXT1:          return (width >> 2U) << 3U;
        case graphics::PF_DXT3:          return (width >> 2U) << 4U;
        case graphics::PF_DXT5:          return (width >> 2U) << 4U;
        case graphics::PF_R5G6B5:
        case graphics::PF_A1R5G5B5:
        case graphics::PF_A4R4G4B4:
        case graphics::PF_COLOR_INDEX4:
        case graphics::PF_COLOR_INDEX8:
        case graphics::PF_COLOR_INDEX16:
        case graphics::PF_COLOR_INDEX32:
        default: CE_ASSERT(false, "D3D11Mappings::GetSysMemPitch(): illegal or unsupported PixelFormat '%i'\n", pf);
    }

    return 0U;
}

//------------------------------------------------------------------------------
/**
*/
graphics::GpuConstantType
D3D11Mappings::Get(const D3D11_SHADER_TYPE_DESC& typeDesc)
{
    switch (typeDesc.Type)
    {
        case D3D_SVT_INT:
            switch (typeDesc.Columns)
            {
                case 1U: return graphics::GCT_INT1;
            }
            break;

        case D3D_SVT_FLOAT:
            switch (typeDesc.Rows)
            {
                case 1U:
                    switch (typeDesc.Columns)
                    {
                        case 1U: return graphics::GCT_FLOAT1;
                        case 3U: return graphics::GCT_FLOAT3;
                        case 4U: return graphics::GCT_FLOAT4;
                    }
                    break;
                case 4U:
                    switch (typeDesc.Columns)
                    {
                        case 4U: return graphics::GCT_MATRIX_4X4;
                    }
                    break;
            }
            break;

        case D3D_SVT_SAMPLER2D: return graphics::GCT_SAMPLER2D;
    }

    CE_ASSERT(false, "D3D11Mappings::Get(): unsupported variable type\n");

    return graphics::GCT_UNKNOWN;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11Mappings::Get(unsigned int colour, float& red, float& green, float& blue, float& alpha)
{
    alpha = ((colour & 0xff000000) >> 24U) / 255.0f;
    blue  = ((colour & 0x00ff0000) >> 16U) / 255.0f;
    green = ((colour & 0x0000ff00) >> 8U) / 255.0f;
    red   = (colour & 0x000000ff) / 255.0f;
}

} // namespace chrissly
