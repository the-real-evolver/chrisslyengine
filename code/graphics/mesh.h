#ifndef MESH_H_
#define MESH_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::Mesh

    (C) 2010 Christian Bleicher
*/
#include "submesh.h"
#include "animation.h"
#include "dynamicarray.h"
#include "hashtable.h"

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
    SubMesh* GetSubMesh(unsigned short index) const;

    /// creates a new Animation object for vertex animating this mesh
    Animation* CreateAnimation(const char* name, float length);
    /// returns the named vertex Animation object
    Animation* GetAnimation(const char* name);
    /// removes all morph Animations from this mesh
    void RemoveAllAnimations();
    /// returns whether or not this mesh has some kind of vertex animation
    bool HasVertexAnimation() const;
    /// initialise an animation set suitable for use with this mesh
    void _InitAnimationState(HashTable* animSet);

private:
    /// a list of submeshes which make up this mesh 
    mutable DynamicArray subMeshList;
    unsigned short numSubMeshes;

    /// storage of morph animations, lookup by name
    HashTable animationsList;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
