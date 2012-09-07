#ifndef SUBMESH_H_
#define SUBMESH_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::SubMesh

    (C) 2011 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class SubMesh
{
public:
    /// default constructor
    SubMesh();
    /// destructor
    ~SubMesh();
    
    unsigned int _vertexCount;
    void* _vertexBuffer;
    
private:
    /// name of the material this SubMesh uses 
    char* materialName;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
