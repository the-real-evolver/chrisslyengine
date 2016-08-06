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
    GLES2HardwareVertexBuffer(unsigned int numVertices, unsigned int bytesPerVertex, graphics::Usage usage);
    /// destructor
    ~GLES2HardwareVertexBuffer();
    /// lock the entire buffer
    void* Lock();
    /// unlock the buffer
    void Unlock();
    /// return gl buffer name
    GLuint GetName() const;

private:
    GLuint bufferName;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
