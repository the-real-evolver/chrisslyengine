//------------------------------------------------------------------------------
//  subentity.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "subentity.h"

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
SubEntity::SubEntity(Entity *parent, SubMesh* subMeshBasis) :
    parentEntity(parent),
    pMaterial(NULL),
    subMesh(subMeshBasis),
    visible(true)
{
    
}

//------------------------------------------------------------------------------
/**
*/
SubEntity::~SubEntity()
{
    
}

//------------------------------------------------------------------------------
/**
*/
void
SubEntity::SetMaterial(Material* material)
{
    this->pMaterial = material;
}
 
//------------------------------------------------------------------------------
/**
*/
Material*
SubEntity::GetMaterial() const
{
    return this->pMaterial;
}

//------------------------------------------------------------------------------
/**
*/
void
SubEntity::SetVisible(bool visible)
{
    this->visible = visible;
}

//------------------------------------------------------------------------------
/**
*/
bool
SubEntity::IsVisible() const
{
    return this->visible;
}

//------------------------------------------------------------------------------
/**
*/
SubMesh*
SubEntity::GetSubMesh()
{
    return this->subMesh;
}

} // namespace graphics
} // namespace chrissly
