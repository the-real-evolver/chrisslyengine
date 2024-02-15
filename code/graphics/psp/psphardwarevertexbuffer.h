#ifndef PSPHARDWAREVERTEXBUFFER_H_
#define PSPHARDWAREVERTEXBUFFER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::PSPHardwareVertexBuffer

    (C) 2016 Christian Bleicher
*/
#include "hardwarevertexbufferbase.h"

//------------------------------------------------------------------------------
namespace chrissly
{

class PSPHardwareVertexBuffer : public graphics::HardwareVertexBufferBase
{
public:
    /// construct from vertex count and vertex size
    PSPHardwareVertexBuffer(unsigned int numVertices, unsigned int bytesPerVertex, graphics::Usage usage, bool useShadowBuffer);
    /// destructor
    ~PSPHardwareVertexBuffer();
    /// enable the cpu to access the data in the buffer and return a pointer to it
    void* Map();
    /// updates the modified data on the the gpu
    void Unmap();

private:
    /// private default constructor
    PSPHardwareVertexBuffer();
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
