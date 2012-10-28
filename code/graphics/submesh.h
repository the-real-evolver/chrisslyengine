#ifndef SUBMESH_H_
#define SUBMESH_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::SubMesh

    (C) 2011 Christian Bleicher
*/
#include "vertexdata.h"
#include "animationtrack.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class SubMesh
{
    friend class Mesh;
    friend class MeshManager;
public:
    /// default constructor
    SubMesh();
    /// destructor
    ~SubMesh();

    /// Get the type of any vertex animation used by dedicated geometry.
    VertexAnimationType GetVertexAnimationType() const;

    /// dedicated vertex data
    VertexData* vertexData;

private:
    /// name of the material this SubMesh uses 
    char* materialName;
    /// type of vertex animation for dedicated vertex data (populated by Mesh)
    VertexAnimationType vertexAnimationType;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
