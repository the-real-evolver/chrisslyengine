#ifndef D3D11MAPPINGS_H_
#define D3D11MAPPINGS_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::D3D11Mappings

    (C) 2016 Christian Bleicher
*/
#include "blendmode.h"
#include "pixelformat.h"
#include "textureunitstate.h"
#include "gpuprogramparams.h"
#include <windows.h>
#include <d3d11.h>
#include <D3DCompiler.h>

//------------------------------------------------------------------------------
namespace chrissly
{
class D3D11Mappings
{
public:
    /// return a D3D11 equivalent for a PrimitiveTopology value
    static D3D_PRIMITIVE_TOPOLOGY Get(graphics::PrimitiveTopology top);
    /// return a D3D11 equivalent for a SceneBlendOperation value
    static D3D11_BLEND_OP Get(graphics::SceneBlendOperation op);
    /// return a D3D11 equivalent for a SceneBlendFactor value
    static D3D11_BLEND Get(graphics::SceneBlendFactor sbf);
    /// return a D3D11 equivalent for a FilterOptions value
    static D3D11_FILTER Get(graphics::FilterOptions minFilter, graphics::FilterOptions magFilter, graphics::FilterOptions mipFilter);
    /// return a D3D11 equivalent for a TextureAddressingMode value
    static D3D11_TEXTURE_ADDRESS_MODE Get(graphics::TextureUnitState::TextureAddressingMode tam);
    /// return a DXGI equivalent for a PixelFormat value
    static DXGI_FORMAT Get(graphics::PixelFormat pf);
    /// return a D3D11 equivalent for a FrameBufferType value
    static UINT Get(unsigned int fbt);
    /// return the distance (in bytes) from the beginning of one line of a texture to the next line
    static UINT GetSysMemPitch(unsigned int width, graphics::PixelFormat pf);
    /// get GpuConstantType from the given d3d11 shader type description
    static graphics::GpuConstantType Get(const D3D11_SHADER_TYPE_DESC& typeDesc);
    /// get rgba components of the given color (Format: AABBGGRR) as float value
    static void Get(unsigned int colour, float& red, float& green, float& blue, float& alpha);
};
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
