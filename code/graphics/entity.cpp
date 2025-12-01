//------------------------------------------------------------------------------
//  entity.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "entity.h"
#include "chrisslyconfig.h"
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
                    HardwareVertexBuffer* subMeshVertexBuffer = subEntity->GetSubMesh()->vertexBuffer;
                    HardwareVertexBuffer* vertexBuffer = CE_NEW HardwareVertexBuffer(subMeshVertexBuffer->GetNumVertices(),
                                                                                    2U * subMeshVertexBuffer->GetBytesPerVertex(), HBU_DYNAMIC, false);
                    subEntity->morphVertexData = vertexBuffer;
                    vertexTrack->ApplyToVertexData(subEntity->morphVertexData, 0);
                }

                it = it->next;
            }
        }
    }

    Skeleton* skeleton = this->mesh->GetSkeleton();
    if (skeleton != NULL)
    {
        // initialise bone matrices with restpose
        ce_array_init(this->boneMatrices, ce_array_size(skeleton->GetParentIndicies()));
        ce_array_header(this->boneMatrices)->size = ce_array_header(this->boneMatrices)->capacity;
        unsigned int numBones = ce_array_size(skeleton->GetLocalTransformMatrices());
        Matrix4* boneLocalMatrix = skeleton->GetLocalTransformMatrices();
        Matrix4* bonesInverseModelMatrix = skeleton->GetInverseModelTransformMatrices();
        int* parentIndex = skeleton->GetParentIndicies();
        CE_ASSERT(numBones <= CE_MAX_BONES_PER_SKELETON, "Entity::Entity(): right now only up to %i bones are supported for skeletal animation, number of bones requested: %i\n", CE_MAX_BONES_PER_SKELETON, numBones);
        static Matrix4 modelTransform[CE_MAX_BONES_PER_SKELETON] = {};
        modelTransform[0U] = boneLocalMatrix[0U];
        unsigned int i;
        for (i = 1U; i < numBones; ++i)
        {
            modelTransform[i] = modelTransform[parentIndex[i]] * boneLocalMatrix[i];
        }
        for (i = 0U; i < numBones; ++i)
        {
            this->boneMatrices[i] = modelTransform[i] * bonesInverseModelMatrix[i];
        }

        // initialise blend matrices
        ce_array_init(this->blendMatrices, ce_array_size(skeleton->GetParentIndicies()));
        for (i = 0U; i < ce_array_size(skeleton->GetParentIndicies()); ++i)
        {
            ce_array_push_back(this->blendMatrices, Matrix4::IDENTITY);
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

    ce_array_delete(this->blendMatrices);
}

//------------------------------------------------------------------------------
/**
*/
SceneNode*
Entity::GetParentSceneNode() const
{
    return this->parentNode;
}

//------------------------------------------------------------------------------
/**
*/
Mesh*
Entity::GetMesh() const
{
    return this->mesh;
}

//------------------------------------------------------------------------------
/**
*/
SubEntity*
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
AnimationState*
Entity::GetAnimationState(const char* const name) const
{
    return (AnimationState*)ce_hash_table_find(&this->animationStates, name, strlen(name));
}

//------------------------------------------------------------------------------
/**
*/
ce_hash_table*
Entity::GetAllAnimationStates() const
{
    return &this->animationStates;
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
        this->blendMatrices[i] = Matrix4::IDENTITY;
    }

    // calculate scaling factor for weights so all weights sum up to 1
    float blendWeightScale = 1.0f;
    Skeleton* skeleton = this->mesh->GetSkeleton();
    if (skeleton != NULL && skeleton->GetBlendMode() == ANIMBLEND_AVERAGE)
    {
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
        blendWeightScale = 1.0f / (weightSum > 0.0f ? weightSum : 1.0f);
    }

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
                anim->Apply(this, state->GetTimePosition(), state->GetWeight() * blendWeightScale, state->GetBlendMask());
            }
            it = it->next;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
chrissly::core::Matrix4*
Entity::GetBoneMatrices() const
{
    return this->boneMatrices;
}

//------------------------------------------------------------------------------
/**
*/
chrissly::core::Matrix4*
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
