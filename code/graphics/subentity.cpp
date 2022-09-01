//------------------------------------------------------------------------------
//  subentity.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "subentity.h"
#include "entity.h"
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
    morphWeight(0.0f),
    currentTimeIndex(-1),
    distanceToCamera(0.0f)
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
SubEntity::SetMaterial(Material* const mat)
{
    this->material = mat;

    if (this->material != NULL)
    {
        if (VAT_MORPH == this->GetSubMesh()->GetVertexAnimationType())
        {
            unsigned short i;
            for (i = 0U; i < this->material->GetNumPasses(); ++i)
            {
                this->material->GetPass(i)->SetMorphAnimationIncluded(true);
            }
        }

        if (this->parentEntity->GetMesh()->GetSkeleton() != NULL)
        {
            unsigned short i;
            for (i = 0U; i < this->material->GetNumPasses(); ++i)
            {
                this->material->GetPass(i)->SetSkeletalAnimationIncluded(true);
            }
        }

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
Entity* const
SubEntity::GetParent() const
{
    return this->parentEntity;
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

//------------------------------------------------------------------------------
/**
*/
void
SubEntity::SetCurrentTimeIndex(int index)
{
    this->currentTimeIndex = index;
}

//------------------------------------------------------------------------------
/**
*/
int
SubEntity::GetCurrentTimeIndex() const
{
    return this->currentTimeIndex;
}

} // namespace graphics
} // namespace chrissly
