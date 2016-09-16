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
D3D11HardwareVertexBuffer::D3D11HardwareVertexBuffer(unsigned int numVertices, unsigned int bytesPerVertex, graphics::Usage usage) :
    HardwareVertexBufferBase(numVertices, bytesPerVertex, usage),
    d3d11Buffer(NULL)
{
    CE_ASSERT(graphics::HBU_STATIC == usage, "D3D11HardwareVertexBuffer::D3D11HardwareVertexBuffer(): dynamic vertexbuffers not supported\n");
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
void*
D3D11HardwareVertexBuffer::Lock()
{
    return HardwareVertexBufferBase::Lock();
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11HardwareVertexBuffer::Unlock()
{
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.ByteWidth = this->numVertices * this->bytesPerVertex;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA data;
    ZeroMemory(&data, sizeof(data));
    data.pSysMem = this->vertexBuffer;

#if __DEBUG__
    HRESULT result =
#endif
    D3D11RenderSystem::Instance()->GetDevice()->CreateBuffer(&desc, &data, &this->d3d11Buffer);
    CE_ASSERT(SUCCEEDED(result), "D3D11HardwareVertexBuffer::Unlock(): failed to create d3d11 buffer\n");
}

//------------------------------------------------------------------------------
/**
*/
ID3D11Buffer*
D3D11HardwareVertexBuffer::GetD3D11Buffer() const
{
    return this->d3d11Buffer;
}

} // namespace chrissly
