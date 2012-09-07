#ifndef MESH_H_
#define MESH_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::Mesh

    (C) 2010 Christian Bleicher
*/
#include "submesh.h"
#include "dynamicarray.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class Mesh
{
public:
    /// default constructor
    Mesh();
    /// destructor
    ~Mesh();
    /// creates a new SubMesh 
    SubMesh* CreateSubMesh();
    /// gets the number of sub meshes which comprise this mesh
    unsigned short GetNumSubMeshes() const;
    /// gets a pointer to the submesh indicated by the index 
    SubMesh* GetSubMesh(unsigned short index);

private:
    /// a list of submeshes which make up this mesh 
    DynamicArray subMeshList;
    unsigned short numSubMeshes;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
