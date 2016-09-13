#ifndef D3D11MAPPINGS_H_
#define D3D11MAPPINGS_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::D3D11Mappings

    (C) 2016 Christian Bleicher
*/
#include "pixelformat.h"
#include <windows.h>
#pragma warning(disable : 4005)
#include <d3dx11.h>

//------------------------------------------------------------------------------
namespace chrissly
{
class D3D11Mappings
{
public:
    /// get rgba components of the given color (Format: AABBGGRR) as float value
    static void Get(unsigned int colour, float& red, float& green, float& blue, float& alpha);
    /// return a DXGI equivalent for a PixelFormat value
    static DXGI_FORMAT Get(graphics::PixelFormat pf);
    /// return the distance (in bytes) from the beginning of one line of a texture to the next line
    static UINT GetSysMemPitch(unsigned int width, graphics::PixelFormat pf);
};
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
