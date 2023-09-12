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
GLES2HardwareVertexBuffer::GLES2HardwareVertexBuffer(unsigned int numVertices, unsigned int bytesPerVertex, graphics::Usage usage, bool useShadowBuffer) :
    HardwareVertexBufferBase(numVertices, bytesPerVertex, usage, useShadowBuffer),
    bufferName(0U)
{
    if (graphics::HBU_STATIC == this->usage && !this->useShadowBuffer)
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
    if (graphics::HBU_STATIC == this->usage && !this->useShadowBuffer)
    {
        glDeleteBuffers(1, &this->bufferName);
        CE_GL_ERROR_CHECK("glDeleteBuffers");
    }
}

//------------------------------------------------------------------------------
/**
*/
void* const
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
    if (graphics::HBU_STATIC == this->usage && !this->useShadowBuffer)
    {
        // workaround to get skeletal animation to work: since glsl 1.00
        // does not support uint attributes we have to convert them to float
        if (this->bytesPerVertex == 68U)
        {
            unsigned char* buffer = (unsigned char*)(this->vertexBuffer) + 16U;
            unsigned int vtx, idx;
            for (vtx = 0U; vtx < this->numVertices; ++vtx)
            {
                unsigned int* uintPtr = (unsigned int*)buffer;
                float* floatPtr = (float*)buffer;
                for (idx = 0U; idx < 4U; ++idx)
                {
                    floatPtr[idx] = (float)uintPtr[idx];
                }
                buffer += this->bytesPerVertex;
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, this->bufferName);
        CE_GL_ERROR_CHECK("glBindBuffer");
        glBufferData(GL_ARRAY_BUFFER, this->numVertices * this->bytesPerVertex, this->vertexBuffer, GL_STATIC_DRAW);
        CE_GL_ERROR_CHECK("glBufferData");
        glBindBuffer(GL_ARRAY_BUFFER, 0U);
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
