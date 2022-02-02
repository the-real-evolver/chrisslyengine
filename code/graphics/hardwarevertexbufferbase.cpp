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
HardwareVertexBufferBase::HardwareVertexBufferBase(unsigned int numVertices, unsigned int bytesPerVertex, Usage usage, bool useShadowBuffer) :
    numVertices(numVertices),
    bytesPerVertex(bytesPerVertex),
    vertexBuffer(NULL),
    usage(usage),
    useShadowBuffer(useShadowBuffer)
{
    if (numVertices > 0U)
    {
        this->vertexBuffer = CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, numVertices * bytesPerVertex);
        CE_ASSERT(this->vertexBuffer != NULL, "HardwareVertexBufferBase::HardwareVertexBufferBase(): failed to allocate '%i' bytes\n", numVertices * bytesPerVertex);
    }
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
HardwareVertexBufferBase::SetNumVertices(unsigned int num)
{
    this->numVertices = num;
}

} // namespace graphics
} // namespace chrissly
