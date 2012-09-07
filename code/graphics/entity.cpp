//------------------------------------------------------------------------------
//  entity.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "entity.h"
#include "memoryallocatorconfig.h"

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
}

//------------------------------------------------------------------------------
/**
*/
SubEntity*
Entity::GetSubEntity(unsigned int index)
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
