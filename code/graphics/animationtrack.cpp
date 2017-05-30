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
    currentTimeIndex(-1)
{
    ce_dynamic_array_init(&this->keyFrames, 1);
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
VertexMorphKeyFrame*
VertexAnimationTrack::CreateVertexMorphKeyFrame(float timePos)
{
    VertexMorphKeyFrame* vertexMorphKeyFrame = CE_NEW VertexMorphKeyFrame(timePos);
    ce_dynamic_array_push_back(&this->keyFrames, vertexMorphKeyFrame);

    return vertexMorphKeyFrame;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
VertexAnimationTrack::GetNumKeyFrames() const
{
    return (unsigned short)this->keyFrames.size;
}

//------------------------------------------------------------------------------
/**
*/
VertexMorphKeyFrame*
VertexAnimationTrack::GetVertexMorphKeyFrame(unsigned short index) const
{
    return (VertexMorphKeyFrame*)ce_dynamic_array_get(&this->keyFrames, index);
}

//------------------------------------------------------------------------------
/**
*/
void
VertexAnimationTrack::RemoveAllKeyFrames()
{
    unsigned int i;
    for (i = 0U; i < this->keyFrames.size; ++i)
    {
        CE_DELETE (VertexMorphKeyFrame*)ce_dynamic_array_get(&this->keyFrames, i);
    }

    ce_dynamic_array_delete(&this->keyFrames);
}

//------------------------------------------------------------------------------
/**
*/
void
VertexAnimationTrack::ApplyToVertexData(VertexData* data, int timeIndex)
{
    if (this->currentTimeIndex != timeIndex)
    {
        if ((unsigned int)timeIndex < this->keyFrames.size - 1)
        {
            HardwareVertexBuffer* kf1 = ((VertexMorphKeyFrame*)ce_dynamic_array_get(&this->keyFrames, timeIndex))->vertexData->vertexBuffer;
            HardwareVertexBuffer* kf2 = ((VertexMorphKeyFrame*)ce_dynamic_array_get(&this->keyFrames, timeIndex + 1))->vertexData->vertexBuffer;
            HardwareVertexBuffer* dst = data->vertexBuffer;
            ce_memory_fill_interleaved(kf1->Map(), kf2->Map(),
                                       dst->Map(), (unsigned short)kf1->GetBytesPerVertex(),
                                       dst->GetBytesPerVertex() * dst->GetNumVertices());
            kf1->Unmap();
            kf2->Unmap();
            dst->Unmap();

            this->currentTimeIndex = timeIndex;
        }
    }
}

} // namespace graphics
} // namespace chrissly
