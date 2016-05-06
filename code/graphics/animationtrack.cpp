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
    numKeyFrames(0),
    currentTimeIndex(-1)
{
    DynamicArrayInit(&this->keyFrames, 0);
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

    if (!DynamicArraySet(&this->keyFrames, this->numKeyFrames, vertexMorphKeyFrame))
    {
        return NULL;
    }

    ++this->numKeyFrames;

    return vertexMorphKeyFrame;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
VertexAnimationTrack::GetNumKeyFrames() const
{
    return this->numKeyFrames;
}

//------------------------------------------------------------------------------
/**
*/
VertexMorphKeyFrame*
VertexAnimationTrack::GetVertexMorphKeyFrame(unsigned short index) const
{
    return (VertexMorphKeyFrame*)DynamicArrayGet(&this->keyFrames, index);
}

//------------------------------------------------------------------------------
/**
*/
void
VertexAnimationTrack::RemoveAllKeyFrames()
{
    unsigned int i;
    for (i = 0; i < this->numKeyFrames; ++i)
    {
        CE_DELETE (VertexMorphKeyFrame*)DynamicArrayGet(&this->keyFrames, i);
    }

    DynamicArrayDelete(&this->keyFrames);

    this->numKeyFrames = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
VertexAnimationTrack::ApplyToVertexData(VertexData* data, int timeIndex)
{
    if (this->currentTimeIndex != timeIndex)
    {
        if (timeIndex < this->numKeyFrames - 1)
        {
            HardwareVertexBuffer* kf1 = ((VertexMorphKeyFrame*)DynamicArrayGet(&this->keyFrames, timeIndex))->vertexData->vertexBuffer;
            HardwareVertexBuffer* kf2 = ((VertexMorphKeyFrame*)DynamicArrayGet(&this->keyFrames, timeIndex + 1))->vertexData->vertexBuffer;
            HardwareVertexBuffer* dst = data->vertexBuffer;
            Memory::FillInterleaved(kf1->Lock(), kf2->Lock(),
                                    dst->Lock(), kf1->GetBytesPerVertex(),
                                    dst->GetBytesPerVertex() * dst->GetNumVertices());
            kf1->Unlock();
            kf2->Unlock();
            dst->Unlock();

            GraphicsSystem::Instance()->GetRenderSystem()->_NotifyMorphKeyFrameBuild();

            this->currentTimeIndex = timeIndex;
        }
    }
}

} // namespace graphics
} // namespace chrissly
