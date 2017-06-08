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
Animation::Animation(const char* const name, float length) :
    length(length)
{
    this->name = name;
    ce_dynamic_array_init(&this->vertexTracks, 1U);
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
    ce_dynamic_array_push_back(&this->vertexTracks, vertexAnimationTrack);

    return vertexAnimationTrack;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
Animation::GetNumVertexTracks() const
{
    return (unsigned short)this->vertexTracks.size;
}

//------------------------------------------------------------------------------
/**
*/
VertexAnimationTrack* const
Animation::GetVertexTrack(unsigned short index) const
{
    return (VertexAnimationTrack*)ce_dynamic_array_get(&this->vertexTracks, index);
}

//------------------------------------------------------------------------------
/**
*/
void
Animation::DestroyAllVertexTracks()
{
    unsigned int i;
    for (i = 0U; i < this->vertexTracks.size; ++i)
    {
        CE_DELETE (VertexAnimationTrack*)ce_dynamic_array_get(&this->vertexTracks, i);
    }

    ce_dynamic_array_delete(&this->vertexTracks);
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
    for (i = 0U; i < this->vertexTracks.size; ++i)
    {
        VertexAnimationTrack* vertexAnimTrack = (VertexAnimationTrack*)ce_dynamic_array_get(&this->vertexTracks, i);
        unsigned short numKeys = vertexAnimTrack->GetNumKeyFrames() - 1U;
        unsigned short keyIndex;
        for (keyIndex = 0; keyIndex < numKeys; ++keyIndex)
        {
            VertexMorphKeyFrame* key = vertexAnimTrack->GetVertexMorphKeyFrame(keyIndex);
            VertexMorphKeyFrame* nextKey = vertexAnimTrack->GetVertexMorphKeyFrame(keyIndex + 1U);
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
