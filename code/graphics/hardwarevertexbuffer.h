#ifndef HARDWAREVERTEXBUFFER_H_
#define HARDWAREVERTEXBUFFER_H_
//------------------------------------------------------------------------------
/**
    Vertexbuffer related stuff

    (C) 2012 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{
    /// Vertex element semantics, used to identify the meaning of vertex buffer contents
    enum VertexElementSemantic
    {
        /// position, 3 reals per vertex
        VES_POSITION = 0,
        /// texture coordinates
        VES_TEXTURE_COORDINATES,
        /// the  number of VertexElementSemantic elements
        VES_COUNT
    };

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif