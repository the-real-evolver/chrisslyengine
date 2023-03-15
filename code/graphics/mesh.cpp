//------------------------------------------------------------------------------
//  mesh.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "mesh.h"
#include "memoryallocatorconfig.h"
#include "animationstate.h"
#include "debug.h"
#include <float.h>

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
Mesh::Mesh() :
    subMeshes(NULL),
    skeleton(NULL),
    boundingRadius(FLT_MAX)
{
    ce_array_init(this->subMeshes, 1U);
    ce_hash_table_init(&this->animations, 0U);
}

//------------------------------------------------------------------------------
/**
*/
Mesh::~Mesh()
{
    this->RemoveAllAnimations();

    if (this->skeleton != NULL)
    {
        CE_DELETE(this->skeleton);
    }

    unsigned int i;
    for (i = 0U; i < ce_array_size(this->subMeshes); ++i)
    {
        CE_DELETE this->subMeshes[i];
    }
    ce_array_delete(this->subMeshes);
}

//------------------------------------------------------------------------------
/**
*/
SubMesh* const
Mesh::CreateSubMesh()
{
    SubMesh* subMesh = CE_NEW SubMesh();
    ce_array_push_back(this->subMeshes, subMesh);

    return subMesh;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
Mesh::GetNumSubMeshes() const
{
    return (unsigned short)ce_array_size(this->subMeshes);
}

//------------------------------------------------------------------------------
/**
*/
SubMesh* const
Mesh::GetSubMesh(unsigned short index) const
{
    return this->subMeshes[index];
}

//------------------------------------------------------------------------------
/**
*/
const core::Vector3&
Mesh::GetBoundingSphereCenter() const
{
    return this->boundingCenter;
}

//------------------------------------------------------------------------------
/**
*/
void
Mesh::SetBoundingSphereRadius(float radius)
{
    this->boundingRadius = radius;
}

//------------------------------------------------------------------------------
/**
*/
float
Mesh::GetBoundingSphereRadius() const
{
    return this->boundingRadius;
}

//------------------------------------------------------------------------------
/**
*/
void
Mesh::SetSkeleton(Skeleton* const skel)
{
    this->skeleton = skel;
}

//------------------------------------------------------------------------------
/**
*/
Skeleton* const
Mesh::GetSkeleton() const
{
    return this->skeleton;
}

//------------------------------------------------------------------------------
/**
*/
Animation* const
Mesh::CreateAnimation(const char* const name, float length)
{
    Animation* anim = (Animation*)ce_hash_table_find(&this->animations, name, strlen(name));
    if (anim != NULL)
    {
        CE_ASSERT(false, "Mesh::CreateAnimation(): animation '%s' already exists, duplicates are not allowed\n", name);
        return NULL;
    }

    Animation* animation = CE_NEW Animation(name, length);

    ce_hash_table_insert(&this->animations, name, strlen(name), animation);

    return animation;
}

//------------------------------------------------------------------------------
/**
*/
Animation* const
Mesh::GetAnimation(const char* const name)
{
    return (Animation*)ce_hash_table_find(&this->animations, name, strlen(name));
}

//------------------------------------------------------------------------------
/**
*/
void
Mesh::RemoveAllAnimations()
{
    unsigned int i;
    for (i = 0U; i < this->animations.bucket_count; ++i)
    {
        ce_linked_list* it = this->animations.buckets[i];
        while (it != NULL)
        {
            CE_DELETE (Animation*)((ce_key_value_pair*)it->data)->value;
            it = it->next;
        }
    }

    ce_hash_table_clear(&this->animations);
}

//------------------------------------------------------------------------------
/**
*/
bool
Mesh::HasAnimation() const
{
    return this->animations.size > 0U;
}

//------------------------------------------------------------------------------
/**
*/
void
Mesh::_InitAnimationState(ce_hash_table* const animSet)
{
    unsigned int i;
    for (i = 0U; i < this->animations.bucket_count; ++i)
    {
        ce_linked_list* it = this->animations.buckets[i];
        while (it != NULL)
        {
            Animation* animation = (Animation*)((ce_key_value_pair*)it->data)->value;
            AnimationState* animationState = CE_NEW AnimationState(animation->GetName(), animation->GetLength());
            ce_hash_table_insert(animSet, animation->GetName(), strlen(animation->GetName()), animationState);
            it = it->next;
        }
    }
}

} // namespace graphics
} // namespace chrissly
