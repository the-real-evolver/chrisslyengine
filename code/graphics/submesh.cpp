//------------------------------------------------------------------------------
//  submesh.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "submesh.h"
#include "memoryallocatorconfig.h"

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
SubMesh::SubMesh()
{

}

//------------------------------------------------------------------------------
/**
*/
SubMesh::~SubMesh()
{
    CE_FREE(this->_vertexBuffer); 
}

} // namespace graphics
} // namespace chrissly
