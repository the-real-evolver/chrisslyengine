#ifndef VERTEXDATA_H_
#define VERTEXDATA_H_
//------------------------------------------------------------------------------
/**
    @struct chrissly::graphics::VertexData

    (C) 2012 Christian Bleicher
*/
#include "hardwarevertexbuffer.h"
#include "memoryallocatorconfig.h"
#include <stdio.h>

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

struct VertexData
{
    /// construct from vertex buffer
    VertexData(HardwareVertexBuffer* vertexBuffer) : vertexBuffer(vertexBuffer) {}
    /// destructor
    ~VertexData() {CE_DELETE this->vertexBuffer;}

    /// pointer to the buffer with the vertexdata
    HardwareVertexBuffer* vertexBuffer;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif