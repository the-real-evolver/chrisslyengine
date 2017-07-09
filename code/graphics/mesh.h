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
    friend class MeshManager;
public:
    /// default constructor
    Mesh();
    /// destructor
    ~Mesh();
    /// creates a new SubMesh
    SubMesh* const CreateSubMesh();
    /// gets the number of sub meshes which comprise this mesh
    unsigned short GetNumSubMeshes() const;
    /// gets a pointer to the submesh indicated by the index
    SubMesh* const GetSubMesh(unsigned short index) const;

    /// creates a new Animation object for vertex animating this mesh
    Animation* const CreateAnimation(const char* const name, float length);
    /// returns the named vertex Animation object
    Animation* const GetAnimation(const char* const name);
    /// removes all morph Animations from this mesh
    void RemoveAllAnimations();
    /// returns whether or not this mesh has some kind of vertex animation
    bool HasVertexAnimation() const;
    /// gets the radius of the bounding sphere surrounding this mesh
    float GetBoundingSphereRadius() const;

    /// initialise an animation set suitable for use with this mesh
    void _InitAnimationState(ce_hash_table* const animSet);

private:
    /// a list of submeshes which make up this mesh
    mutable ce_dynamic_array subMeshes;
    /// storage of morph animations, lookup by name
    ce_hash_table animations;
    /// local bounding sphere radius (centered on object)
    float boundingRadius;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
