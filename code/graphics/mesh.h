#ifndef MESH_H_
#define MESH_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::Mesh

    (C) 2010 Christian Bleicher
*/
#include "submesh.h"
#include "skeleton.h"
#include "animation.h"
#include "hashtable.h"
#include "vector3.h"

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
    SubMesh* CreateSubMesh();
    /// gets the number of sub meshes which comprise this mesh
    unsigned short GetNumSubMeshes() const;
    /// gets a pointer to the submesh indicated by the index
    SubMesh* GetSubMesh(unsigned short index) const;

    /// gets the center of the bounding sphere surrounding this mesh
    const core::Vector3& GetBoundingSphereCenter() const;
    /// sets the radius of the bounding sphere surrounding this mesh (useful for manual created meshes)
    void SetBoundingSphereRadius(float radius);
    /// gets the radius of the bounding sphere surrounding this mesh
    float GetBoundingSphereRadius() const;

    /// set the skeleton this mesh uses for animation
    void SetSkeleton(Skeleton* const skel);
    /// gets a pointer to the linked skeleton
    Skeleton* GetSkeleton() const;

    /// creates a new Animation object for vertex animating this mesh
    Animation* CreateAnimation(const char* const name, float length);
    /// returns the named vertex Animation object
    Animation* GetAnimation(const char* const name);
    /// removes all morph Animations from this mesh
    void RemoveAllAnimations();
    /// returns whether or not this mesh has some kind of animation
    bool HasAnimation() const;
    /// initialise an animation set suitable for use with this mesh
    void _InitAnimationState(ce_hash_table* const animSet) const;

private:
    /// a list of submeshes which make up this mesh
    SubMesh** subMeshes;
    /// pointer to a skeleton
    Skeleton* skeleton;
    /// storage of morph animations, lookup by name
    ce_hash_table animations;
    /// local bounding sphere center
    core::Vector3 boundingCenter;
    /// local bounding sphere radius
    float boundingRadius;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
