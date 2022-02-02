//------------------------------------------------------------------------------
//  d3d11hardwarevertexbuffer.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "d3d11hardwarevertexbuffer.h"
#include "d3d11rendersystem.h"
#include "debug.h"
#include <windows.h>

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
D3D11HardwareVertexBuffer::D3D11HardwareVertexBuffer(unsigned int numVertices, unsigned int bytesPerVertex, graphics::Usage usage, bool useShadowBuffer) :
    HardwareVertexBufferBase(numVertices, bytesPerVertex, usage, useShadowBuffer),
    d3d11Buffer(NULL)
{
    if (graphics::HBU_DYNAMIC == this->usage)
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.ByteWidth = this->numVertices * this->bytesPerVertex;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

#if __CE_DEBUG__
        HRESULT result =
#endif
        D3D11RenderSystem::Instance()->GetDevice()->CreateBuffer(&desc, NULL, &this->d3d11Buffer);
        CE_ASSERT(SUCCEEDED(result), "D3D11HardwareVertexBuffer::D3D11HardwareVertexBuffer(): failed to create d3d11 buffer\n");
    }
}

//------------------------------------------------------------------------------
/**
*/
D3D11HardwareVertexBuffer::~D3D11HardwareVertexBuffer()
{
    if (this->d3d11Buffer != NULL)
    {
        this->d3d11Buffer->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
void* const
D3D11HardwareVertexBuffer::Map()
{
    if (graphics::HBU_STATIC == this->usage)
    {
        return this->vertexBuffer;
    }
    else if (graphics::HBU_DYNAMIC == this->usage)
    {
        D3D11_MAPPED_SUBRESOURCE resource;
#if __CE_DEBUG__
        HRESULT result =
#endif
        D3D11RenderSystem::Instance()->GetContext()->Map(this->d3d11Buffer, 0U, D3D11_MAP_WRITE_DISCARD, 0U, &resource);
        CE_ASSERT(SUCCEEDED(result), "D3D11HardwareVertexBuffer::Map(): failed to map d3d11 buffer\n");
        return resource.pData;
    }

    return NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11HardwareVertexBuffer::Unmap()
{
    if (graphics::HBU_STATIC == this->usage && NULL == this->d3d11Buffer && !this->useShadowBuffer)
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.ByteWidth = this->numVertices * this->bytesPerVertex;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA data;
        ZeroMemory(&data, sizeof(data));
        data.pSysMem = this->vertexBuffer;

#if __CE_DEBUG__
        HRESULT result =
#endif
        D3D11RenderSystem::Instance()->GetDevice()->CreateBuffer(&desc, &data, &this->d3d11Buffer);
        CE_ASSERT(SUCCEEDED(result), "D3D11HardwareVertexBuffer::Unmap(): failed to create d3d11 buffer\n");
    }
    else if (graphics::HBU_DYNAMIC == this->usage)
    {
        D3D11RenderSystem::Instance()->GetContext()->Unmap(this->d3d11Buffer, 0U);
    }
}

//------------------------------------------------------------------------------
/**
*/
ID3D11Buffer* const
D3D11HardwareVertexBuffer::GetD3D11Buffer() const
{
    return this->d3d11Buffer;
}

} // namespace chrissly
