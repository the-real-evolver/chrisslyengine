//------------------------------------------------------------------------------
//  mesh.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "mesh.h"
#include "memoryallocatorconfig.h"

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
Mesh::Mesh() : numSubMeshes(0)
{
    DynamicArrayInit(&this->subMeshList, 0);    
}

//------------------------------------------------------------------------------
/**
*/
Mesh::~Mesh()
{
    unsigned int i;
    for (i = 0; i < this->numSubMeshes; i++)
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
   
    if (!DynamicArraySet(&this->subMeshList, this->numSubMeshes, subMesh))
    {
        return NULL;
    }
    
    this->numSubMeshes++;
    
    return subMesh;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
Mesh::GetNumSubMeshes() const
{
    return this->numSubMeshes;
}

//------------------------------------------------------------------------------
/**
*/
SubMesh*
Mesh::GetSubMesh(unsigned short index)
{
    return (SubMesh*)DynamicArrayGet(&this->subMeshList, index);
}

} // namespace graphics
} // namespace chrissly
