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
Entity::Entity(Mesh* const mesh) :
    parentNode(NULL),
    mesh(mesh),
    subEntities(NULL),
    visible(true),
    castShadows(false),
    receivesShadows(false),
    boneMatrices(NULL),
    blendMatrices(NULL)
{
    ce_hash_table_init(&this->animationStates, 0U);

    this->BuildSubEntities();

    if (this->HasAnimation())
    {
        this->mesh->_InitAnimationState(&this->animationStates);

        unsigned int i;
        for (i = 0U; i < this->animationStates.bucket_count; ++i)
        {
            ce_linked_list* it = this->animationStates.buckets[i];
            while (it != NULL)
            {
                AnimationState* state = (AnimationState*)((ce_key_value_pair*)it->data)->value;
                Animation* anim = this->mesh->GetAnimation(state->GetAnimationName());

                unsigned short trackIndex;
                for (trackIndex = 0U; trackIndex < anim->GetNumVertexTracks(); ++trackIndex)
                {
                    VertexAnimationTrack* vertexTrack = anim->GetVertexTrack(trackIndex);
                    SubEntity* subEntity = this->GetSubEntity(vertexTrack->GetHandle());
                    CE_ASSERT(subEntity != NULL, "Entity::Entity(): subEntity not valid\n");
                    HardwareVertexBuffer* subMeshVertexBuffer = subEntity->GetSubMesh()->vertexData->vertexBuffer;
                    HardwareVertexBuffer* vertexBuffer = CE_NEW HardwareVertexBuffer(subMeshVertexBuffer->GetNumVertices(),
                                                                                    2U * subMeshVertexBuffer->GetBytesPerVertex(), HBU_DYNAMIC, false);
                    subEntity->morphVertexData = CE_NEW VertexData(vertexBuffer);
                    vertexTrack->ApplyToVertexData(subEntity->morphVertexData, 0);
                }

                it = it->next;
            }
        }
    }

    Skeleton* skeleton = this->mesh->GetSkeleton();
    if (skeleton != NULL)
    {
        ce_array_init(this->boneMatrices, ce_array_size(skeleton->GetParentIndicies()));
        ce_array_init(this->blendMatrices, ce_array_size(skeleton->GetParentIndicies()));
        unsigned int i;
        for (i = 0U; i < ce_array_size(skeleton->GetParentIndicies()); ++i)
        {
            ce_array_push_back(this->boneMatrices, Matrix4::IDENTITY);
            ce_array_push_back(this->blendMatrices, Matrix4::ZERO);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
Entity::~Entity()
{
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->subEntities); ++i)
    {
        CE_DELETE this->subEntities[i];
    }
    ce_array_delete(this->subEntities);

    for (i = 0U; i < this->animationStates.bucket_count; ++i)
    {
        ce_linked_list* it = this->animationStates.buckets[i];
        while (it != NULL)
        {
            CE_DELETE (AnimationState*)((ce_key_value_pair*)it->data)->value;
            it = it->next;
        }
    }
    ce_hash_table_clear(&this->animationStates);

    ce_array_delete(this->boneMatrices);
}

//------------------------------------------------------------------------------
/**
*/
SceneNode* const
Entity::GetParentSceneNode() const
{
    return this->parentNode;
}

//------------------------------------------------------------------------------
/**
*/
Mesh* const
Entity::GetMesh() const
{
    return this->mesh;
}

//------------------------------------------------------------------------------
/**
*/
SubEntity* const
Entity::GetSubEntity(unsigned int index) const
{
    return this->subEntities[index];
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
Entity::GetNumSubEntities() const
{
    return ce_array_size(this->subEntities);
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::SetVisible(bool vis)
{
    this->visible = vis;
}

//------------------------------------------------------------------------------
/**
*/
bool
Entity::IsVisible() const
{
    return this->visible;
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
Entity::HasAnimation() const
{
    return this->mesh->HasAnimation();
}

//------------------------------------------------------------------------------
/**
*/
AnimationState* const
Entity::GetAnimationState(const char* const name) const
{
    return (AnimationState*)ce_hash_table_find(&this->animationStates, name, strlen(name));
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::UpdateAnimation()
{
    // reset blend matrices
    unsigned int i, numMatrices = ce_array_size(this->blendMatrices);
    for (i = 0U; i < numMatrices; ++i)
    {
        this->blendMatrices[i] = Matrix4::ZERO;
    }

    // calculate scaling factor for weights so all weights sum up to 1
    float weightSum = 0.0f;
    for (i = 0U; i < this->animationStates.bucket_count; ++i)
    {
        ce_linked_list* it = this->animationStates.buckets[i];
        while (it != NULL)
        {
            AnimationState* state = (AnimationState*)((ce_key_value_pair*)it->data)->value;
            if (state->GetEnabled())
            {
                weightSum += state->GetWeight();
            }
            it = it->next;
        }
    }
    float blendWeightScale = 1.0f / (weightSum > 0.0f ? weightSum : 1.0f);

    // loop trough all animstates, update enabled ones
    for (i = 0U; i < this->animationStates.bucket_count; ++i)
    {
        ce_linked_list* it = this->animationStates.buckets[i];
        while (it != NULL)
        {
            AnimationState* state = (AnimationState*)((ce_key_value_pair*)it->data)->value;
            if (state->GetEnabled())
            {
                Animation* anim = this->mesh->GetAnimation(state->GetAnimationName());
                anim->Apply(this, state->GetTimePosition(), state->GetWeight() * blendWeightScale);
            }
            it = it->next;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
chrissly::core::Matrix4* const
Entity::GetBoneMatrices() const
{
    return this->boneMatrices;
}

//------------------------------------------------------------------------------
/**
*/
chrissly::core::Matrix4* const
Entity::GetBlendMatrices() const
{
    return this->blendMatrices;
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::_NotifyAttached(SceneNode* const parent)
{
    this->parentNode = parent;
}

//------------------------------------------------------------------------------
/**
*/
void
Entity::BuildSubEntities()
{
    ce_array_init(this->subEntities, this->mesh->GetNumSubMeshes());

    unsigned int i;
    for (i = 0U; i < ce_array_header(this->subEntities)->capacity; ++i)
    {
        SubMesh* subMesh = this->mesh->GetSubMesh((unsigned short)i);
        SubEntity* subEntity = CE_NEW SubEntity(this, subMesh);
        subEntity->SetMaterialName(subMesh->GetMaterialName());
        ce_array_push_back(this->subEntities, subEntity);
    }
}

} // namespace graphics
} // namespace chrissly
