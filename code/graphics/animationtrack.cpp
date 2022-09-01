//------------------------------------------------------------------------------
//  animationtrack.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "animationtrack.h"
#include "graphicssystem.h"
#include "memoryallocatorconfig.h"
#include "chrisslymemory.h"

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
VertexAnimationTrack::VertexAnimationTrack(unsigned char handle) :
    handle(handle),
    keyFrames(NULL)
{
    ce_array_init(this->keyFrames, 1U);
}

//------------------------------------------------------------------------------
/**
*/
VertexAnimationTrack::~VertexAnimationTrack()
{
    this->RemoveAllKeyFrames();
}

//------------------------------------------------------------------------------
/**
*/
unsigned char
VertexAnimationTrack::GetHandle() const
{
    return this->handle;
}

//------------------------------------------------------------------------------
/**
*/
VertexMorphKeyFrame* const
VertexAnimationTrack::CreateVertexMorphKeyFrame(float timePos)
{
    VertexMorphKeyFrame* vertexMorphKeyFrame = CE_NEW VertexMorphKeyFrame(timePos);
    ce_array_push_back(this->keyFrames, vertexMorphKeyFrame);

    return vertexMorphKeyFrame;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
VertexAnimationTrack::GetNumKeyFrames() const
{
    return (unsigned short)ce_array_size(this->keyFrames);
}

//------------------------------------------------------------------------------
/**
*/
VertexMorphKeyFrame* const
VertexAnimationTrack::GetVertexMorphKeyFrame(unsigned short index) const
{
    return this->keyFrames[index];
}

//------------------------------------------------------------------------------
/**
*/
void
VertexAnimationTrack::RemoveAllKeyFrames()
{
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->keyFrames); ++i)
    {
        CE_DELETE this->keyFrames[i];
    }
    ce_array_delete(this->keyFrames);
}

//------------------------------------------------------------------------------
/**
*/
void
VertexAnimationTrack::ApplyToVertexData(VertexData* const data, int timeIndex)
{
    if ((unsigned int)timeIndex < ce_array_size(this->keyFrames) - 1U)
    {
        HardwareVertexBuffer* kf1 = this->keyFrames[timeIndex]->vertexData->vertexBuffer;
        HardwareVertexBuffer* kf2 = this->keyFrames[timeIndex + 1]->vertexData->vertexBuffer;
        HardwareVertexBuffer* dst = data->vertexBuffer;
        ce_memory_fill_interleaved(kf1->Map(), kf2->Map(),
                                   dst->Map(), (unsigned short)kf1->GetBytesPerVertex(),
                                   dst->GetBytesPerVertex() * dst->GetNumVertices());
        kf1->Unmap();
        kf2->Unmap();
        dst->Unmap();
    }
}

//------------------------------------------------------------------------------
/**
*/
BoneAnimationTrack::BoneAnimationTrack(unsigned int numKeyFrames)
{
    ce_array_init(this->timeIndex, numKeyFrames);
    ce_array_init(this->localMatrix, numKeyFrames);
}

//------------------------------------------------------------------------------
/**
*/
BoneAnimationTrack::~BoneAnimationTrack()
{
    ce_array_delete(this->timeIndex);
    ce_array_delete(this->localMatrix);
}

//------------------------------------------------------------------------------
/**
*/
void
BoneAnimationTrack::CreateKeyFrame(float timePos, chrissly::core::Matrix4 const& transform)
{
    ce_array_push_back(this->timeIndex, timePos);
    ce_array_push_back(this->localMatrix, transform);
}

//------------------------------------------------------------------------------
/**
*/
chrissly::core::Matrix4* const
BoneAnimationTrack::GetTransformMatrices() const
{
    return this->localMatrix;
}

//------------------------------------------------------------------------------
/**
*/
float* const
BoneAnimationTrack::GetTimeIndicies() const
{
    return this->timeIndex;
}

} // namespace graphics
} // namespace chrissly
