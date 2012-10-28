#ifndef VERTEXDATA_H_
#define VERTEXDATA_H_
//------------------------------------------------------------------------------
/**
    @struct chrissly::graphics::VertexData

    (C) 2012 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

struct VertexData
{
    /// number of vertices
    unsigned int vertexCount;
    /// pointer to the buffer with the vertexdata
    void* vertexBuffer;
    /// size of a vertex in bytes
    unsigned int bytesPerVertex;

    VertexData(unsigned int vertexCount, void* vertexBuffer, unsigned int bytesPerVertex) :
        vertexCount(vertexCount), vertexBuffer(vertexBuffer), bytesPerVertex(bytesPerVertex) {}
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif