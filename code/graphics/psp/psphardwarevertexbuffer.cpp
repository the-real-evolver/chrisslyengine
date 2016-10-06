//------------------------------------------------------------------------------
//  psphardwarevertexbuffer.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "psphardwarevertexbuffer.h"
#include <psputils.h>

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
PSPHardwareVertexBuffer::PSPHardwareVertexBuffer()
{

}

//------------------------------------------------------------------------------
/**
*/
PSPHardwareVertexBuffer::PSPHardwareVertexBuffer(unsigned int numVertices, unsigned int bytesPerVertex, graphics::Usage usage, bool useShadowBuffer) :
    HardwareVertexBufferBase(numVertices, bytesPerVertex, usage, useShadowBuffer)
{

}

//------------------------------------------------------------------------------
/**
*/
PSPHardwareVertexBuffer::~PSPHardwareVertexBuffer()
{

}

//------------------------------------------------------------------------------
/**
*/
void*
PSPHardwareVertexBuffer::Map()
{
    return this->vertexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
void
PSPHardwareVertexBuffer::Unmap()
{
    if (graphics::HBU_DYNAMIC == this->usage)
    {
        sceKernelDcacheWritebackRange(this->vertexBuffer, this->numVertices * this->bytesPerVertex);
    }
}

} // namespace chrissly
