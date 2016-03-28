//------------------------------------------------------------------------------
//  keyframe.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "keyframe.h"
#include "memoryallocatorconfig.h"
#include <stdio.h>

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
VertexMorphKeyFrame::VertexMorphKeyFrame(float time) :
    vertexData(NULL),
    time(time)
{

}

//------------------------------------------------------------------------------
/**
*/
VertexMorphKeyFrame::~VertexMorphKeyFrame()
{
    CE_FREE(this->vertexData->vertexBuffer);
    CE_DELETE this->vertexData;
}

//------------------------------------------------------------------------------
/**
*/
float
VertexMorphKeyFrame::GetTime() const
{
    return this->time;
}

} // namespace graphics
} // namespace chrissly
