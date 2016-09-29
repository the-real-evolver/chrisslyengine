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
PSPHardwareVertexBuffer::PSPHardwareVertexBuffer(unsigned int numVertices, unsigned int bytesPerVertex, graphics::Usage usage) :
    HardwareVertexBufferBase(numVertices, bytesPerVertex, usage)
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
        sceKernelDcacheWritebackAll();
    }
}

} // namespace chrissly
