//------------------------------------------------------------------------------
//  gles2hardwarevertexbuffer.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "gles2hardwarevertexbuffer.h"
#include "gles2debug.h"

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
GLES2HardwareVertexBuffer::GLES2HardwareVertexBuffer()
{

}

//------------------------------------------------------------------------------
/**
*/
GLES2HardwareVertexBuffer::GLES2HardwareVertexBuffer(unsigned int numVertices, unsigned int bytesPerVertex, graphics::Usage usage) :
    HardwareVertexBufferBase(numVertices, bytesPerVertex, usage),
    bufferName(0)
{
    if (graphics::HBU_STATIC == this->usage)
    {
        glGenBuffers(1, &this->bufferName);
        CE_GL_ERROR_CHECK("glGenBuffers");
    }
}

//------------------------------------------------------------------------------
/**
*/
GLES2HardwareVertexBuffer::~GLES2HardwareVertexBuffer()
{
    if (graphics::HBU_STATIC == this->usage)
    {
        glDeleteBuffers(1, &this->bufferName);
        CE_GL_ERROR_CHECK("glDeleteBuffers");
    }
}

//------------------------------------------------------------------------------
/**
*/
void*
GLES2HardwareVertexBuffer::Map()
{
    return this->vertexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2HardwareVertexBuffer::Unmap()
{
    if (graphics::HBU_STATIC == this->usage)
    {
        glBindBuffer(GL_ARRAY_BUFFER, this->bufferName);
        CE_GL_ERROR_CHECK("glBindBuffer");
        glBufferData(GL_ARRAY_BUFFER, this->numVertices * this->bytesPerVertex, this->vertexBuffer, GL_STATIC_DRAW);
        CE_GL_ERROR_CHECK("glBufferData");
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CE_GL_ERROR_CHECK("glBindBuffer");
    }
}

//------------------------------------------------------------------------------
/**
*/
GLuint
GLES2HardwareVertexBuffer::GetName() const
{
    return this->bufferName;
}

} // namespace chrissly
