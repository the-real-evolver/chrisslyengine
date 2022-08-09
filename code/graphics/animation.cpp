//------------------------------------------------------------------------------
//  animation.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "animation.h"
#include "entity.h"
#include "chrisslyarray.h"
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
Animation::Animation(const char* const name, float length) :
    length(length),
    vertexTracks(NULL)
{
    this->name = name;
    ce_array_init(this->vertexTracks, 1U);
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
const char* const
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
VertexAnimationTrack* const
Animation::CreateVertexTrack(unsigned char handle)
{
    VertexAnimationTrack* vertexAnimationTrack = CE_NEW VertexAnimationTrack(handle);
    ce_array_push_back(this->vertexTracks, vertexAnimationTrack);

    return vertexAnimationTrack;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
Animation::GetNumVertexTracks() const
{
    return (unsigned short)ce_array_size(this->vertexTracks);
}

//------------------------------------------------------------------------------
/**
*/
VertexAnimationTrack* const
Animation::GetVertexTrack(unsigned short index) const
{
    return this->vertexTracks[index];
}

//------------------------------------------------------------------------------
/**
*/
void
Animation::DestroyAllVertexTracks()
{
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->vertexTracks); ++i)
    {
        CE_DELETE this->vertexTracks[i];
    }
    ce_array_delete(this->vertexTracks);
}

//------------------------------------------------------------------------------
/**
*/
void
Animation::Apply(Entity* const entity, float timePos)
{
    if (timePos > this->length && this->length > 0.0f)
    {
        timePos = Math::Fmod(timePos, this->length);
    }

    unsigned int i;
    for (i = 0U; i < ce_array_size(this->vertexTracks); ++i)
    {
        VertexAnimationTrack* vertexAnimTrack = this->vertexTracks[i];
        unsigned short numKeys = vertexAnimTrack->GetNumKeyFrames() - 1U;
        unsigned short keyIndex;
        for (keyIndex = 0U; keyIndex < numKeys; ++keyIndex)
        {
            VertexMorphKeyFrame* key = vertexAnimTrack->GetVertexMorphKeyFrame(keyIndex);
            VertexMorphKeyFrame* nextKey = vertexAnimTrack->GetVertexMorphKeyFrame(keyIndex + 1U);
            if (timePos >= key->GetTime() && timePos < nextKey->GetTime())
            {
                SubEntity* subEntity = entity->GetSubEntity(vertexAnimTrack->GetHandle());
                subEntity->SetMorphWeight((timePos - key->GetTime()) / (nextKey->GetTime() - key->GetTime()));
                if (subEntity->GetCurrentTimeIndex() != keyIndex)
                {
                    vertexAnimTrack->ApplyToVertexData(subEntity->GetMorphVertexData(), keyIndex);
                    subEntity->SetCurrentTimeIndex(keyIndex);
                }
                break;
            }
        }
    }
}

} // namespace graphics
} // namespace chrissly
