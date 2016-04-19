#ifndef SUBMESH_H_
#define SUBMESH_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::SubMesh

    (C) 2011 Christian Bleicher
*/
#include "vertexdata.h"
#include "animationtrack.h"
#include "chrisslystring.h"

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
    /// sets the name of the Material which this SubMesh will use
    void SetMaterialName(const core::String& matName);
    /// gets the name of the Material this SubMesh is using
    const core::String& GetMaterialName() const;
    /// get the type of any vertex animation used by dedicated geometry
    VertexAnimationType GetVertexAnimationType() const;

    /// dedicated vertex data
    VertexData* vertexData;

private:
    /// name of the material this SubMesh uses
    core::String materialName;
    /// type of vertex animation for dedicated vertex data (populated by Mesh)
    VertexAnimationType vertexAnimationType;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
