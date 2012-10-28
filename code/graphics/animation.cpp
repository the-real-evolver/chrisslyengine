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

//------------------------------------------------------------------------------
/**
*/
Animation::Animation(const char* name, float length) :
    name(name),
    length(length),
    numVertexTracks(0)
{
    DynamicArrayInit(&this->vertexTrackList, 0);
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
    return this->name;
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
Animation::CreateVertexTrack()
{
    VertexAnimationTrack* vertexAnimationTrack = CE_NEW VertexAnimationTrack();

    if (!DynamicArraySet(&this->vertexTrackList, this->numVertexTracks, vertexAnimationTrack))
    {
        return NULL;
    }

    this->numVertexTracks++;

    return vertexAnimationTrack;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
Animation::GetNumVertexTracks() const
{
    return this->numVertexTracks;
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
    for (i = 0; i < this->numVertexTracks; i++)
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
    SubEntity* subEntity = entity->GetSubEntity(0);
    VertexData* hwVertexData = subEntity->_GetHardwareVertexAnimVertexData();

    if (timePos > this->length && this->length > 0.0f)
    {
        timePos = core::Math::Fmod(timePos, this->length);
    }

    int timeIndex = 0;
    float morphWeight = 0.0f;
    unsigned int i;
    for (i = 0; i < this->numVertexTracks; i++)
    {
        VertexAnimationTrack* vertexAnimTrack = (VertexAnimationTrack*)DynamicArrayGet(&this->vertexTrackList, i);
        unsigned short numKeys = vertexAnimTrack->GetNumKeyFrames() - 1;
        unsigned short keyIndex;
        for (keyIndex = 0; keyIndex < numKeys; keyIndex++)
        {
            VertexMorphKeyFrame* key = vertexAnimTrack->GetVertexMorphKeyFrame(keyIndex);
            VertexMorphKeyFrame* nextKey = vertexAnimTrack->GetVertexMorphKeyFrame(keyIndex + 1);
            if (timePos >= key->GetTime() && timePos < nextKey->GetTime())
            {
                timeIndex = keyIndex;
                morphWeight = (timePos - key->GetTime()) / (nextKey->GetTime() - key->GetTime());
                break;
            }
        }
    }

    subEntity->SetMorphWeight(morphWeight);

    for (i = 0; i < this->numVertexTracks; i++)
    {
        VertexAnimationTrack* vertexAnimTrack = (VertexAnimationTrack*)DynamicArrayGet(&this->vertexTrackList, i);
        vertexAnimTrack->ApplyToVertexData(hwVertexData, timeIndex);
    }
}

} // namespace graphics
} // namespace chrissly
