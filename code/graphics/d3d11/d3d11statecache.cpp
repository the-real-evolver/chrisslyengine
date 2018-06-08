//------------------------------------------------------------------------------
//  d3d11statecache.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "d3d11statecache.h"
#include "d3d11rendersystem.h"
#include "debug.h"

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
D3D11StateCache::D3D11StateCache()
{
    ce_hash_table_init(&this->rasterizerStates, 2U);
    ce_hash_table_init(&this->depthStencilStates, 2U);
    ce_hash_table_init(&this->blendStates, 2U);
    ce_hash_table_init(&this->samplerStates, 2U);
}

//------------------------------------------------------------------------------
/**
*/
D3D11StateCache::~D3D11StateCache()
{
    this->Invalidate();
}

//------------------------------------------------------------------------------
/**
*/
ID3D11RasterizerState* const
D3D11StateCache::GetRasterizerState(const D3D11_RASTERIZER_DESC& desc)
{
    ID3D11RasterizerState* state = (ID3D11RasterizerState*)ce_hash_table_find(&this->rasterizerStates, (const char*)&desc, sizeof(desc));
    if (state != NULL)
    {
        return state;
    }

#if __CE_DEBUG__
    HRESULT result =
#endif
    D3D11RenderSystem::Instance()->GetDevice()->CreateRasterizerState(&desc, &state);
    CE_ASSERT(SUCCEEDED(result), "D3D11StateCache::GetRasterizerState(): failed to create rasterizer state\n");

    ce_hash_table_insert(&this->rasterizerStates, (const char*)&desc, sizeof(desc), state);

    return state;
}

//------------------------------------------------------------------------------
/**
*/
ID3D11DepthStencilState* const
D3D11StateCache::GetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& desc)
{
    ID3D11DepthStencilState* state = (ID3D11DepthStencilState*)ce_hash_table_find(&this->depthStencilStates, (const char*)&desc, sizeof(desc));
    if (state != NULL)
    {
        return state;
    }

#if __CE_DEBUG__
    HRESULT result =
#endif
    D3D11RenderSystem::Instance()->GetDevice()->CreateDepthStencilState(&desc, &state);
    CE_ASSERT(SUCCEEDED(result), "D3D11StateCache::GetDepthStencilState(): failed to create depth stencil state\n");

    ce_hash_table_insert(&this->depthStencilStates, (const char*)&desc, sizeof(desc), state);

    return state;
}

//------------------------------------------------------------------------------
/**
*/
ID3D11BlendState* const
D3D11StateCache::GetBlendState(const D3D11_BLEND_DESC& desc)
{
    ID3D11BlendState* state = (ID3D11BlendState*)ce_hash_table_find(&this->blendStates, (const char*)&desc, sizeof(desc));
    if (state != NULL)
    {
        return state;
    }

#if __CE_DEBUG__
    HRESULT result =
#endif
    D3D11RenderSystem::Instance()->GetDevice()->CreateBlendState(&desc, &state);
    CE_ASSERT(SUCCEEDED(result), "D3D11StateCache::GetBlendState(): failed to create blend state\n");

    ce_hash_table_insert(&this->blendStates, (const char*)&desc, sizeof(desc), state);

    return state;
}

//------------------------------------------------------------------------------
/**
*/
ID3D11SamplerState* const
D3D11StateCache::GetSamplerState(const D3D11_SAMPLER_DESC& desc)
{
    ID3D11SamplerState* state = (ID3D11SamplerState*)ce_hash_table_find(&this->samplerStates, (const char*)&desc, sizeof(desc));
    if (state != NULL)
    {
        return state;
    }

#if __CE_DEBUG__
    HRESULT result =
#endif
    D3D11RenderSystem::Instance()->GetDevice()->CreateSamplerState(&desc, &state);
    CE_ASSERT(SUCCEEDED(result), "D3D11StateCache::GetSamplerState(): failed to create sampler state\n");

    ce_hash_table_insert(&this->samplerStates, (const char*)&desc, sizeof(desc), state);

    return state;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11StateCache::Invalidate()
{
    unsigned int i;

    for (i = 0U; i < this->rasterizerStates.bucket_count; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(&this->rasterizerStates, i);
        while (it != NULL)
        {
            ID3D11RasterizerState* state = (ID3D11RasterizerState*)((ce_key_value_pair*)it->data)->value;
            state->Release();
            it = it->next;
        }
    }
    ce_hash_table_clear(&this->rasterizerStates);

    for (i = 0U; i < this->depthStencilStates.bucket_count; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(&this->depthStencilStates, i);
        while (it != NULL)
        {
            ID3D11DepthStencilState* state = (ID3D11DepthStencilState*)((ce_key_value_pair*)it->data)->value;
            state->Release();
            it = it->next;
        }
    }
    ce_hash_table_clear(&this->depthStencilStates);

    for (i = 0U; i < this->blendStates.bucket_count; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(&this->blendStates, i);
        while (it != NULL)
        {
            ID3D11BlendState* state = (ID3D11BlendState*)((ce_key_value_pair*)it->data)->value;
            state->Release();
            it = it->next;
        }
    }
    ce_hash_table_clear(&this->blendStates);

    for (i = 0U; i < this->samplerStates.bucket_count; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(&this->samplerStates, i);
        while (it != NULL)
        {
            ID3D11SamplerState* state = (ID3D11SamplerState*)((ce_key_value_pair*)it->data)->value;
            state->Release();
            it = it->next;
        }
    }
    ce_hash_table_clear(&this->samplerStates);
}

} // namespace chrissly
