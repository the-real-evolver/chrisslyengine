#ifndef D3D11HARDWAREVERTEXBUFFER_H_
#define D3D11HARDWAREVERTEXBUFFER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::D3D11HardwareVertexBuffer

    (C) 2016 Christian Bleicher
*/
#include "hardwarevertexbufferbase.h"
#pragma warning(disable : 4005)
#include <d3dx11.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class D3D11HardwareVertexBuffer : public graphics::HardwareVertexBufferBase
{
public:
    /// construct from vertex count and vertex size
    D3D11HardwareVertexBuffer(unsigned int numVertices, unsigned int bytesPerVertex, graphics::Usage usage);
    /// destructor
    ~D3D11HardwareVertexBuffer();
    /// lock the entire buffer
    void* Lock();
    /// unlock the buffer
    void Unlock();

    /// gets a pointer to the d3d11 buffer
    ID3D11Buffer* GetD3D11Buffer() const;

private:
    ID3D11Buffer* d3d11Buffer;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
