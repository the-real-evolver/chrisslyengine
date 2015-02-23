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
SubEntity::SubEntity(Entity *parent, SubMesh* subMeshBasis) :
    parentEntity(parent),
    pMaterial(NULL),
    subMesh(subMeshBasis),
    visible(true),
    hardwareVertexAnimVertexData(NULL),
    morphWeight(0.0f)
{

}

//------------------------------------------------------------------------------
/**
*/
SubEntity::~SubEntity()
{
    if (this->hardwareVertexAnimVertexData != NULL)
    {
        CE_FREE(this->hardwareVertexAnimVertexData->vertexBuffer);
        CE_DELETE this->hardwareVertexAnimVertexData;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SubEntity::SetMaterialName(const String& matName)
{
    Material* material = MaterialManager::Instance()->GetByName(matName.C_Str());
    this->SetMaterial(material);
}

//------------------------------------------------------------------------------
/**
*/
void
SubEntity::SetMaterial(Material* material)
{
    this->pMaterial = material;

    if (this->pMaterial != NULL)
    {
        this->pMaterial->Load();
    }
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

//------------------------------------------------------------------------------
/**
*/
VertexData*
SubEntity::_GetHardwareVertexAnimVertexData() const
{
    return this->hardwareVertexAnimVertexData;
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
