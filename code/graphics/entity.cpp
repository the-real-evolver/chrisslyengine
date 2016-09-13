//------------------------------------------------------------------------------
//  entity.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "entity.h"
#include "memoryallocatorconfig.h"
#include "debug.h"

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
Entity::Entity(Mesh* mesh) :
    parentNode(NULL),
    mesh(mesh),
    castShadows(false),
    receivesShadows(false)
{
    this->BuildSubEntities();

    ce_hash_table_init(&this->animationState, 1);
    if (this->HasVertexAnimation())
    {
        this->mesh->_InitAnimationState(&this->animationState);

        unsigned int i;
        for (i = 0; i < this->animationState.bucket_count; ++i)
        {
            ce_linked_list* it = ce_hash_table_begin(&this->animationState, i);
            while (it != NULL)
            {
                AnimationState* state = (AnimationState*)((ce_key_value_pair*)it->data)->value;
                Animation* anim = this->mesh->GetAnimation(state->GetAnimationName());

                unsigned short numVertexTracks = anim->GetNumVertexTracks();
                unsigned short trackIndex;
                for (trackIndex = 0; trackIndex < numVertexTracks; ++trackIndex)
                {
                    VertexAnimationTrack* vertexTrack = anim->GetVertexTrack(trackIndex);
                    SubEntity* subEntity = this->GetSubEntity(vertexTrack->GetHandle());
                    CE_ASSERT(subEntity != NULL, "Entity::Entity(): subEntity not valid\n");
                    VertexData* subMeshVertexData = subEntity->GetSubMesh()->vertexData;
                    HardwareVertexBuffer* vertexBuffer = CE_NEW HardwareVertexBuffer(subMeshVertexData->vertexBuffer->GetNumVertices(),
                                                                                    2 * subMeshVertexData->vertexBuffer->GetBytesPerVertex(), HBU_DYNAMIC);
                    subEntity->hardwareVertexAnimVertexData = CE_NEW VertexData(vertexBuffer);
                }

                it = it->next;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
Entity::~Entity()
{
    unsigned int i;
    for (i = 0; i < this->subEntities.capacity; ++i)
    {
        CE_DELETE (SubEntity*)ce_dynamic_array_get(&this->subEntities, i);
    }

    ce_dynamic_array_delete(&this->subEntities);

    for (i = 0; i < this->animationState.bucket_count; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(&this->animationState, i);
        while (it != NULL)
        {
            CE_DELETE (AnimationState*)((ce_key_value_pair*)it->data)->value;
            it = it->next;
        }
    }

    ce_hash_table_clear(&this->animationState);
}

//------------------------------------------------------------------------------
/**
*/
SubEntity*
Entity::GetSubEntity(unsigned int index) const
{
    return (SubEntity*)ce_dynamic_array_get(&this->subEntities, index);
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
Entity::GetNumSubEntities() const
{
    return this->subEntities.capacity;
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::_NotifyAttached(SceneNode* parent)
{
    this->parentNode = parent;
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::SetCastShadows(bool enabled)
{
    this->castShadows = enabled;
}

//------------------------------------------------------------------------------
/**
*/
bool
Entity::GetCastShadows() const
{
    return this->castShadows;
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::SetReceivesShadows(bool enabled)
{
    this->receivesShadows = enabled;
}

//------------------------------------------------------------------------------
/**
*/
bool
Entity::GetReceivesShadows() const
{
    return this->receivesShadows;
}

//------------------------------------------------------------------------------
/**
*/
bool
Entity::HasVertexAnimation() const
{
    return this->mesh->HasVertexAnimation();
}

//------------------------------------------------------------------------------
/**
*/
AnimationState*
Entity::GetAnimationState(const char* name) const
{
    return (AnimationState*)ce_hash_table_find(&this->animationState, name);
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::UpdateAnimation()
{
    // loop trough all animstates, update enabled ones
    unsigned int i;
    for (i = 0; i < this->animationState.bucket_count; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(&this->animationState, i);
        while (it != NULL)
        {
            AnimationState* state = (AnimationState*)((ce_key_value_pair*)it->data)->value;
            if (state->GetEnabled())
            {
                Animation* anim = this->mesh->GetAnimation(state->GetAnimationName());
                anim->Apply(this, state->GetTimePosition());
            }
            it = it->next;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::BuildSubEntities()
{
    ce_dynamic_array_init(&this->subEntities, this->mesh->GetNumSubMeshes());

    unsigned int i;
    for (i = 0; i < this->subEntities.capacity; ++i)
    {
        SubMesh* subMesh = this->mesh->GetSubMesh((unsigned short)i);
        SubEntity* subEntity = CE_NEW SubEntity(this, subMesh);
        subEntity->SetMaterialName(subMesh->GetMaterialName());
        ce_dynamic_array_set(&this->subEntities, i, subEntity);
    }
}

} // namespace graphics
} // namespace chrissly
