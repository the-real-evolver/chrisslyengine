//------------------------------------------------------------------------------
//  hardwarevertexbufferbase.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "hardwarevertexbufferbase.h"
#include "memoryallocatorconfig.h"
#include "debug.h"
#include <stdio.h>

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
HardwareVertexBufferBase::HardwareVertexBufferBase()
{

}

//------------------------------------------------------------------------------
/**
*/
HardwareVertexBufferBase::HardwareVertexBufferBase(unsigned int numVertices, unsigned int bytesPerVertex)
{
    if (numVertices > 0)
    {
        this->vertexBuffer = CE_MALLOC_ALIGN(16, numVertices * bytesPerVertex);
        CE_ASSERT(this->vertexBuffer != NULL, "HardwareVertexBufferBase::HardwareVertexBufferBase(): failed to allocate '%i' bytes\n", numVertices * bytesPerVertex);
    }
    else
    {
        this->vertexBuffer = NULL;
    }

    this->numVertices = numVertices;
    this->bytesPerVertex = bytesPerVertex;
}

//------------------------------------------------------------------------------
/**
*/
HardwareVertexBufferBase::~HardwareVertexBufferBase()
{
    if (this->vertexBuffer != NULL)
    {
        CE_FREE(this->vertexBuffer);
    }
}

 //------------------------------------------------------------------------------
/**
*/
void*
HardwareVertexBufferBase::Lock()
{
    return this->vertexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
void
HardwareVertexBufferBase::Unlock()
{

}

//------------------------------------------------------------------------------
/**
*/
unsigned int
HardwareVertexBufferBase::GetBytesPerVertex() const
{
    return this->bytesPerVertex;
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
HardwareVertexBufferBase::GetNumVertices() const
{
    return this->numVertices;
}

//------------------------------------------------------------------------------
/**
*/
void
HardwareVertexBufferBase::SetNumVertices(unsigned int numVertices)
{
    this->numVertices = numVertices;
}

} // namespace graphics
} // namespace chrissly
