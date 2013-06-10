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
    static MeshManager* Instance()
    {
        return Singleton;
    }

    /// default constructor
    MeshManager();
    /// destructor
    ~MeshManager();
    /// loads a mesh from a file, making it immediately available for use
    Mesh* Load(const char* filename);
    /// removes all resources
    void RemoveAll();

private:
    /// copy constructor
    MeshManager(const MeshManager&cc) {}; 

    static MeshManager* Singleton;

    chrissly::core::HashTable resources;

    enum MeshChunkID
    {
        M_SUBMESH = 0x01,
        M_ANIMATION = 0x02,
        M_ANIMATION_TRACK = 0x03,
        M_ANIMATION_MORPH_KEYFRAME = 0x04
    };
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
