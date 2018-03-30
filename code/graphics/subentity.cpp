//------------------------------------------------------------------------------
//  subentity.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "subentity.h"
#include "materialmanager.h"

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
SubEntity::SubEntity(Entity* parent, SubMesh* subMeshBasis) :
    parentEntity(parent),
    material(NULL),
    subMesh(subMeshBasis),
    visible(true),
    morphVertexData(NULL),
    morphWeight(0.0f)
{

}

//------------------------------------------------------------------------------
/**
*/
SubEntity::~SubEntity()
{
    if (this->morphVertexData != NULL)
    {
        CE_DELETE this->morphVertexData;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SubEntity::SetMaterialName(const String& matName)
{
    Material* mat = MaterialManager::Instance()->GetByName(matName.C_Str());
    this->SetMaterial(mat);
}

//------------------------------------------------------------------------------
/**
*/
void
SubEntity::SetMaterial(Material* mat)
{
    this->material = mat;

    if (this->material != NULL)
    {
        this->material->Load();
    }
}
 
//------------------------------------------------------------------------------
/**
*/
Material* const
SubEntity::GetMaterial() const
{
    return this->material;
}

//------------------------------------------------------------------------------
/**
*/
void
SubEntity::SetVisible(bool vis)
{
    this->visible = vis;
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
SubMesh* const
SubEntity::GetSubMesh() const
{
    return this->subMesh;
}

//------------------------------------------------------------------------------
/**
*/
VertexData* const
SubEntity::GetMorphVertexData() const
{
    return this->morphVertexData;
}

//------------------------------------------------------------------------------
/**
*/
void
SubEntity::SetMorphWeight(float weight)
{
    this->morphWeight = weight;
}

//------------------------------------------------------------------------------
/**
*/
float
SubEntity::GetMorphWeight() const
{
    return this->morphWeight;
}

} // namespace graphics
} // namespace chrissly
