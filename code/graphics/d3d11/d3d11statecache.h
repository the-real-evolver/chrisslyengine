#ifndef D3D11STATECACHE_H_
#define D3D11STATECACHE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::D3D11StateCache

    (C) 2016 Christian Bleicher
*/
#include "hashtable.h"
#pragma warning(disable : 4005)
#include <d3dx11.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class D3D11StateCache
{
public:
    /// default constructor
    D3D11StateCache();
    /// destructor
    ~D3D11StateCache();

    /// returns a rasterizer state object for the given description
    ID3D11RasterizerState* const GetRasterizerState(const D3D11_RASTERIZER_DESC& desc);
    /// returns a depth stencil state object for the given description
    ID3D11DepthStencilState* const GetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& desc);
    /// returns a blend state object for the given description
    ID3D11BlendState* const GetBlendState(const D3D11_BLEND_DESC& desc);
    /// returns a sampler state object for the given description
    ID3D11SamplerState* const GetSamplerState(const D3D11_SAMPLER_DESC& desc);
    /// invalidates/clears the cache
    void Invalidate();

private:
    ce_hash_table rasterizerStates;
    ce_hash_table depthStencilStates;
    ce_hash_table blendStates;
    ce_hash_table samplerStates;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
