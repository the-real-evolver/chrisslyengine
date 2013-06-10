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
    length(length),
    numVertexTracks(0)
{
	this->name = name;
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

    this->numVertexTracks = 0;
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
                SubEntity* subEntity = entity->GetSubEntity(0);
                subEntity->SetMorphWeight((timePos - key->GetTime()) / (nextKey->GetTime() - key->GetTime()));
                vertexAnimTrack->ApplyToVertexData(subEntity->_GetHardwareVertexAnimVertexData(), keyIndex);
                break;
            }
        }
    }
}

} // namespace graphics
} // namespace chrissly
