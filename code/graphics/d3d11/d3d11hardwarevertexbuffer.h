#ifndef D3D11HARDWAREVERTEXBUFFER_H_
#define D3D11HARDWAREVERTEXBUFFER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::D3D11HardwareVertexBuffer

    (C) 2016 Christian Bleicher
*/
#include "hardwarevertexbufferbase.h"
#include <d3d11.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class D3D11HardwareVertexBuffer : public graphics::HardwareVertexBufferBase
{
public:
    /// construct from vertex count and vertex size
    D3D11HardwareVertexBuffer(unsigned int numVertices, unsigned int bytesPerVertex, graphics::Usage usage, bool useShadowBuffer);
    /// destructor
    ~D3D11HardwareVertexBuffer();
    /// enable the cpu to access the data in the buffer and return a pointer to it
    void* const Map();
    /// updates the modified data on the the gpu
    void Unmap();

    /// gets a pointer to the d3d11 buffer
    ID3D11Buffer* const GetD3D11Buffer() const;

private:
    /// private default constructor
    D3D11HardwareVertexBuffer();

    ID3D11Buffer* d3d11Buffer;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
