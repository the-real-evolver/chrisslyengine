#ifndef GLES2HARDWAREVERTEXBUFFER_H_
#define GLES2HARDWAREVERTEXBUFFER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::GLES2HardwareVertexBuffer

    Encapsulates a OpenGL Vertex Buffer Object (VBO).

    (C) 2016 Christian Bleicher
*/
#include "hardwarevertexbufferbase.h"
#include <GLES2/gl2.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class GLES2HardwareVertexBuffer : public graphics::HardwareVertexBufferBase
{
public:
    /// construct from vertex count and vertex size
    GLES2HardwareVertexBuffer(unsigned int numVertices, unsigned int bytesPerVertex, graphics::Usage usage, bool useShadowBuffer);
    /// destructor
    ~GLES2HardwareVertexBuffer();
    /// enable the cpu to access the data in the buffer and return a pointer to it
    void* Map();
    /// updates the modified data on the the gpu
    void Unmap();
    /// return gl buffer name
    GLuint GetName() const;

private:
    /// private default constructor
    GLES2HardwareVertexBuffer();

    GLuint bufferName;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
