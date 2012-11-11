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

//------------------------------------------------------------------------------
/**
*/
Entity::Entity(const char* name, Mesh* mesh) :
    castShadows(false),
    receivesShadows(false)
{
    this->mesh = mesh;

    this->numSubEntities = 0;
    DynamicArrayInit(&this->subEntityList, 0);
    this->BuildSubEntityList(this->mesh, &this->subEntityList);

    HashTableInit(&this->animationState, 1);
    if (this->HasVertexAnimation())
    {
        this->mesh->_InitAnimationState(&this->animationState);

        // Fixme: setup for multiple animationtracks
        SubEntity* subEntity = this->GetSubEntity(0);
        CE_ASSERT(subEntity != NULL, "Entity::Entity(): subEntity not valid");
        VertexData* subMeshVertexData = subEntity->GetSubMesh()->vertexData;
        void* vertexBuffer = CE_MALLOC(2 * subMeshVertexData->bytesPerVertex * subMeshVertexData->vertexCount);
        subEntity->hardwareVertexAnimVertexData = CE_NEW VertexData(subMeshVertexData->vertexCount, vertexBuffer, subMeshVertexData->bytesPerVertex);
    }
}

//------------------------------------------------------------------------------
/**
*/
Entity::~Entity()
{
    unsigned int i;
    for (i = 0; i < this->numSubEntities; i++)
    {
        CE_DELETE (SubEntity*)DynamicArrayGet(&this->subEntityList, i);
    }

    DynamicArrayDelete(&this->subEntityList);

    for (i = 0; i < this->animationState.capacity; i++)
    {
        Chain* chain = (Chain*)DynamicArrayGet(&this->animationState.entries, i);
        LinkedList* it = chain->list;
        while (it != NULL)
        {
            CE_DELETE (AnimationState*)((KeyValuePair*)it->data)->value;
            it = it->next;
        }
    }

    HashTableClear(&this->animationState);
}

//------------------------------------------------------------------------------
/**
*/
SubEntity*
Entity::GetSubEntity(unsigned int index) const
{
    return (SubEntity*)DynamicArrayGet(&this->subEntityList, index);
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
Entity::GetNumSubEntities() const
{
    return this->numSubEntities;
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
    return (AnimationState*)HashTableFind(&this->animationState, name);
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::UpdateAnimation()
{
    // loop trough all animstates, update enabled ones
    unsigned int i;
    for (i = 0; i < this->animationState.capacity; i++)
    {
        Chain* chain = (Chain*)DynamicArrayGet(&this->animationState.entries, i);
        LinkedList* it = chain->list;
        while (it != NULL)
        {
            AnimationState* state = (AnimationState*)((KeyValuePair*)it->data)->value;
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
Entity::BuildSubEntityList(Mesh* mesh, DynamicArray* sublist)
{
    unsigned int i;
    for (i = 0; i < mesh->GetNumSubMeshes(); i++)
    {
        SubEntity* subEntity = CE_NEW SubEntity(this, mesh->GetSubMesh(i));
        // ToDo: set material
        DynamicArraySet(sublist, i, subEntity);
        this->numSubEntities++;
    }
}

} // namespace graphics
} // namespace chrissly
