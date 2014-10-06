//------------------------------------------------------------------------------
//  submesh.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "submesh.h"
#include "memoryallocatorconfig.h"
#include <stdio.h>

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
SubMesh::SubMesh() :
    vertexData(NULL),
    vertexAnimationType(VAT_NONE)
{

}

//------------------------------------------------------------------------------
/**
*/
SubMesh::~SubMesh()
{
    if (this->vertexData->vertexBuffer != NULL)
    {
        CE_FREE(this->vertexData->vertexBuffer);
    }
    CE_DELETE this->vertexData;
}

//------------------------------------------------------------------------------
/**
*/
void
SubMesh::SetMaterialName(const String& matName)
{
    this->materialName = matName;
}

//------------------------------------------------------------------------------
/**
*/
const String&
SubMesh::GetMaterialName() const
{
    return this->materialName;
}

//------------------------------------------------------------------------------
/**
*/
VertexAnimationType
SubMesh::GetVertexAnimationType() const
{
    return this->vertexAnimationType;
}

} // namespace graphics
} // namespace chrissly
