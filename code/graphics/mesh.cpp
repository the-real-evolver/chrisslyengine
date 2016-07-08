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
    DynamicArrayInit(&this->subMeshList, 1);
    HashTableInit(&this->animationsList, 1);
}

//------------------------------------------------------------------------------
/**
*/
Mesh::~Mesh()
{
    this->RemoveAllAnimations();

    unsigned int i;
    for (i = 0; i < this->subMeshList.size; ++i)
    {
        CE_DELETE (SubMesh*)DynamicArrayGet(&this->subMeshList, i);
    }

    DynamicArrayDelete(&this->subMeshList);
}

//------------------------------------------------------------------------------
/**
*/
SubMesh*
Mesh::CreateSubMesh()
{
    SubMesh* subMesh = CE_NEW SubMesh();
    DynamicArrayPushBack(&this->subMeshList, subMesh);

    return subMesh;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
Mesh::GetNumSubMeshes() const
{
    return this->subMeshList.size;
}

//------------------------------------------------------------------------------
/**
*/
SubMesh*
Mesh::GetSubMesh(unsigned short index) const
{
    return (SubMesh*)DynamicArrayGet(&this->subMeshList, index);
}

//------------------------------------------------------------------------------
/**
*/
Animation*
Mesh::CreateAnimation(const char* name, float length)
{
    Animation* animation = CE_NEW Animation(name, length);

    HashTableInsert(&this->animationsList, name, animation);

    return animation;
}

//------------------------------------------------------------------------------
/**
*/
Animation*
Mesh::GetAnimation(const char* name)
{
    return (Animation*)HashTableFind(&this->animationsList, name);
}

//------------------------------------------------------------------------------
/**
*/
void
Mesh::RemoveAllAnimations()
{
    unsigned int i;
    for (i = 0; i < this->animationsList.bucketCount; ++i)
    {
        LinkedList* it = HashTableBegin(&this->animationsList, i);
        while (it != NULL)
        {
            CE_DELETE (Animation*)((KeyValuePair*)it->data)->value;
            it = it->next;
        }
    }

    HashTableClear(&this->animationsList);
}

//------------------------------------------------------------------------------
/**
*/
bool
Mesh::HasVertexAnimation() const
{
    return (this->animationsList.currentSize > 0);
}

//------------------------------------------------------------------------------
/**
*/
void
Mesh::_InitAnimationState(HashTable* animSet)
{
    unsigned int i;
    for (i = 0; i < this->animationsList.bucketCount; ++i)
    {
        LinkedList* it = HashTableBegin(&this->animationsList, i);
        while (it != NULL)
        {
            Animation* animation = (Animation*)((KeyValuePair*)it->data)->value;
            AnimationState* animationState = CE_NEW AnimationState(animation->GetName(), animation->GetLength());
            HashTableInsert(animSet, animation->GetName(), animationState);
            it = it->next;
        }
    }
}

} // namespace graphics
} // namespace chrissly
