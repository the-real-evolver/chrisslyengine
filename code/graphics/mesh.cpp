//------------------------------------------------------------------------------
//  mesh.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "mesh.h"
#include "memoryallocatorconfig.h"
#include "animationstate.h"

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
Mesh::Mesh()
{
    ce_dynamic_array_init(&this->subMeshes, 1);
    ce_hash_table_init(&this->animations, 1);
}

//------------------------------------------------------------------------------
/**
*/
Mesh::~Mesh()
{
    this->RemoveAllAnimations();

    unsigned int i;
    for (i = 0; i < this->subMeshes.size; ++i)
    {
        CE_DELETE (SubMesh*)ce_dynamic_array_get(&this->subMeshes, i);
    }

    ce_dynamic_array_delete(&this->subMeshes);
}

//------------------------------------------------------------------------------
/**
*/
SubMesh*
Mesh::CreateSubMesh()
{
    SubMesh* subMesh = CE_NEW SubMesh();
    ce_dynamic_array_push_back(&this->subMeshes, subMesh);

    return subMesh;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
Mesh::GetNumSubMeshes() const
{
    return this->subMeshes.size;
}

//------------------------------------------------------------------------------
/**
*/
SubMesh*
Mesh::GetSubMesh(unsigned short index) const
{
    return (SubMesh*)ce_dynamic_array_get(&this->subMeshes, index);
}

//------------------------------------------------------------------------------
/**
*/
Animation*
Mesh::CreateAnimation(const char* name, float length)
{
    Animation* animation = CE_NEW Animation(name, length);

    ce_hash_table_insert(&this->animations, name, animation);

    return animation;
}

//------------------------------------------------------------------------------
/**
*/
Animation*
Mesh::GetAnimation(const char* name)
{
    return (Animation*)ce_hash_table_find(&this->animations, name);
}

//------------------------------------------------------------------------------
/**
*/
void
Mesh::RemoveAllAnimations()
{
    unsigned int i;
    for (i = 0; i < this->animations.bucket_count; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(&this->animations, i);
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
Mesh::HasVertexAnimation() const
{
    return (this->animations.size > 0);
}

//------------------------------------------------------------------------------
/**
*/
void
Mesh::_InitAnimationState(ce_hash_table* animSet)
{
    unsigned int i;
    for (i = 0; i < this->animations.bucket_count; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(&this->animations, i);
        while (it != NULL)
        {
            Animation* animation = (Animation*)((ce_key_value_pair*)it->data)->value;
            AnimationState* animationState = CE_NEW AnimationState(animation->GetName(), animation->GetLength());
            ce_hash_table_insert(animSet, animation->GetName(), animationState);
            it = it->next;
        }
    }
}

} // namespace graphics
} // namespace chrissly
