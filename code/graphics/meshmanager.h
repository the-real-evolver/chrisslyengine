#ifndef MESHMANAGER_H_
#define MESHMANAGER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::MeshManager

    (C) 2010 Christian Bleicher
*/
#include "mesh.h"
#include "hashtable.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class MeshManager
{
public:
    /// get pointer to the singleton
    static MeshManager* const Instance()
    {
        return Singleton;
    }

    /// default constructor
    MeshManager();
    /// destructor
    ~MeshManager();
    /// loads a mesh from a file, making it immediately available for use
    Mesh* Load(const char* const filename);
    /// creates an empty mesh and registers it with the given name, intended for programmatically mesh creation (like particlesystems etc.)
    Mesh* CreateManual(const char* const name);
    /// removes all resources
    void RemoveAll();
    /// returns a pointer to the hashtable that contains all meshes
    ce_hash_table* GetAllMeshes() const;

private:
    /// copy constructor
    MeshManager(const MeshManager&);
    /// prevent copy by assignment
    MeshManager& operator = (const MeshManager&);

    static MeshManager* Singleton;

    mutable ce_hash_table resources;

    enum MeshChunkID
    {
        M_SUBMESH = 0x01,
        M_MESH_BOUNDS = 0x02,
        M_ANIMATION = 0x03,
        M_ANIMATION_TRACK = 0x04,
        M_ANIMATION_MORPH_KEYFRAME = 0x05,
        M_MESH_SKELETON_FILE = 0x06
    };
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
