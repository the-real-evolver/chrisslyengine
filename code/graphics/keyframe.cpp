//------------------------------------------------------------------------------
//  keyframe.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "keyframe.h"
#include "memoryallocatorconfig.h"

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
VertexMorphKeyFrame::VertexMorphKeyFrame(float time) : time(time)
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