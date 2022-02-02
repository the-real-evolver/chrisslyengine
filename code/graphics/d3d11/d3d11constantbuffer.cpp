//------------------------------------------------------------------------------
//  d3d11ConstantBuffer.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "d3d11constantbuffer.h"
#include "d3d11rendersystem.h"
#include "debug.h"

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
D3D11ConstantBuffer::D3D11ConstantBuffer(UINT byteWidth, UINT slot, unsigned int numConstants) :
    buffer(NULL),
    slot(slot)
{
    ce_dynamic_array_init(&this->constants, numConstants);

    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0U;
    desc.ByteWidth = byteWidth;
#if __CE_DEBUG__
    HRESULT result =
#endif
    D3D11RenderSystem::Instance()->GetDevice()->CreateBuffer(&desc, NULL, &this->buffer);
    CE_ASSERT(SUCCEEDED(result), "D3D11ConstantBuffer::D3D11ConstantBuffer(): failed to create constant buffer\n");
}

//------------------------------------------------------------------------------
/**
*/
D3D11ConstantBuffer::~D3D11ConstantBuffer()
{
    ce_dynamic_array_delete(&this->constants);

    if (this->buffer != NULL)
    {
        this->buffer->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11ConstantBuffer::Bind()
{
    ID3D11DeviceContext* context = D3D11RenderSystem::Instance()->GetContext();
    context->VSSetConstantBuffers(this->slot, 1U, &this->buffer);
    context->PSSetConstantBuffers(this->slot, 1U, &this->buffer);
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11ConstantBuffer::UpdateConstants()
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(mappedResource));
    ID3D11DeviceContext* context = D3D11RenderSystem::Instance()->GetContext();
    context->Map(this->buffer, 0U, D3D11_MAP_WRITE_DISCARD, 0U, &mappedResource);

    unsigned int i;
    for (i = 0U; i < this->constants.size; ++i)
    {
        graphics::GpuConstantDefinition* variable = (graphics::GpuConstantDefinition*)ce_dynamic_array_get(&this->constants, i);
        memcpy((void*)((UINT_PTR)mappedResource.pData + (UINT_PTR)variable->location), variable->buffer, variable->size * variable->arraySize);
    }

    context->Unmap(this->buffer, 0U);
}

} // namespace chrissly
