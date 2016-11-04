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
void
D3D11Mappings::Get(unsigned int colour, float& red, float& green, float& blue, float& alpha)
{
    alpha = ((colour & 0xff000000) >> 24) / 255.0f;
    blue  = ((colour & 0x00ff0000) >> 16) / 255.0f;
    green = ((colour & 0x0000ff00) >> 8) / 255.0f;
    red   = (colour & 0x000000ff) / 255.0f;
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
D3D11Mappings::GetSysMemPitch(unsigned int width, graphics::PixelFormat pf)
{
    switch (pf)
    {
        case graphics::PF_R8G8B8A8:      return width << 2;
        case graphics::PF_DXT1:          return (width >> 2) << 3;
        case graphics::PF_DXT3:          return (width >> 2) << 4;
        case graphics::PF_DXT5:          return (width >> 2) << 4;
        case graphics::PF_R5G6B5:
        case graphics::PF_A1R5G5B5:
        case graphics::PF_A4R4G4B4:
        case graphics::PF_COLOR_INDEX4:
        case graphics::PF_COLOR_INDEX8:
        case graphics::PF_COLOR_INDEX16:
        case graphics::PF_COLOR_INDEX32:
        default: CE_ASSERT(false, "D3D11Mappings::GetSysMemPitch(): illegal or unsupported PixelFormat '%i'\n", pf);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
graphics::GpuConstantType
D3D11Mappings::Get(D3D11_SHADER_TYPE_DESC& typeDesc)
{
    switch (typeDesc.Type)
    {
        case D3D_SVT_INT:
            switch (typeDesc.Columns)
            {
                case 1: return graphics::GCT_INT1;
            }
            break;
        case D3D_SVT_FLOAT:
            switch (typeDesc.Rows)
            {
                case 1:
                    switch (typeDesc.Columns)
                    {
                        case 1: return graphics::GCT_FLOAT1;
                        case 3: return graphics::GCT_FLOAT3;
                        case 4: return graphics::GCT_FLOAT4;
                    }
                case 4:
                    switch (typeDesc.Columns)
                    {
                        case 4: return graphics::GCT_MATRIX_4X4;
                    }
            }
            break;
        case D3D_SVT_SAMPLER2D: return graphics::GCT_SAMPLER2D;
    }

    CE_ASSERT(false, "D3D11GpuProgram::ExtractConstantDefs(): unsupported variable type\n");

    return graphics::GCT_UNKNOWN;
}

} // namespace chrissly
