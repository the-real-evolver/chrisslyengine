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
        case graphics::PF_DXT3:          return DXGI_FORMAT_BC3_UNORM;
        case graphics::PF_DXT5:          return DXGI_FORMAT_BC5_UNORM;
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
        case graphics::PF_R8G8B8A8:      return width * 4;
        case graphics::PF_DXT1:          return (width + 1) / 2;
        case graphics::PF_DXT3:          return width;
        case graphics::PF_DXT5:          return width;
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

} // namespace chrissly
