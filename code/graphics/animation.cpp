//------------------------------------------------------------------------------
//  animation.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "animation.h"
#include "entity.h"
#include "chrisslymath.h"
#include "memoryallocatorconfig.h"

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
Animation::Animation(const char* name, float length) :
    length(length)
{
    this->name = name;
    DynamicArrayInit(&this->vertexTrackList, 1);
}

//------------------------------------------------------------------------------
/**
*/
Animation::~Animation()
{
    this->DestroyAllVertexTracks();
}

//------------------------------------------------------------------------------
/**
*/
const char*
Animation::GetName() const
{
    return this->name.C_Str();
}

//------------------------------------------------------------------------------
/**
*/
float
Animation::GetLength() const
{
    return this->length;
}

//------------------------------------------------------------------------------
/**
*/
VertexAnimationTrack*
Animation::CreateVertexTrack(unsigned char handle)
{
    VertexAnimationTrack* vertexAnimationTrack = CE_NEW VertexAnimationTrack(handle);
    DynamicArrayPushBack(&this->vertexTrackList, vertexAnimationTrack);

    return vertexAnimationTrack;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
Animation::GetNumVertexTracks() const
{
    return this->vertexTrackList.size;
}

//------------------------------------------------------------------------------
/**
*/
VertexAnimationTrack*
Animation::GetVertexTrack(unsigned short index) const
{
    return (VertexAnimationTrack*)DynamicArrayGet(&this->vertexTrackList, index);
}

//------------------------------------------------------------------------------
/**
*/
void
Animation::DestroyAllVertexTracks()
{
    unsigned int i;
    for (i = 0; i < this->vertexTrackList.size; ++i)
    {
        CE_DELETE (VertexAnimationTrack*)DynamicArrayGet(&this->vertexTrackList, i);
    }

    DynamicArrayDelete(&this->vertexTrackList);
}

//------------------------------------------------------------------------------
/**
*/
void
Animation::Apply(Entity* entity, float timePos)
{
    if (timePos > this->length && this->length > 0.0f)
    {
        timePos = Math::Fmod(timePos, this->length);
    }

    unsigned int i;
    for (i = 0; i < this->vertexTrackList.size; ++i)
    {
        VertexAnimationTrack* vertexAnimTrack = (VertexAnimationTrack*)DynamicArrayGet(&this->vertexTrackList, i);
        unsigned short numKeys = vertexAnimTrack->GetNumKeyFrames() - 1;
        unsigned short keyIndex;
        for (keyIndex = 0; keyIndex < numKeys; ++keyIndex)
        {
            VertexMorphKeyFrame* key = vertexAnimTrack->GetVertexMorphKeyFrame(keyIndex);
            VertexMorphKeyFrame* nextKey = vertexAnimTrack->GetVertexMorphKeyFrame(keyIndex + 1);
            if (timePos >= key->GetTime() && timePos < nextKey->GetTime())
            {
                SubEntity* subEntity = entity->GetSubEntity(vertexAnimTrack->GetHandle());
                subEntity->SetMorphWeight((timePos - key->GetTime()) / (nextKey->GetTime() - key->GetTime()));
                vertexAnimTrack->ApplyToVertexData(subEntity->_GetHardwareVertexAnimVertexData(), keyIndex);
                break;
            }
        }
    }
}

} // namespace graphics
} // namespace chrissly
