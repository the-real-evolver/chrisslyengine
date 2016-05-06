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
        /// normal, 3 reals per vertex
        VES_NORMAL,
        /// texture coordinates
        VES_TEXTURE_COORDINATES,
        /// position of the morph target vertex
        VES_POSITION_MORPH_TARGET,
        /// the  number of VertexElementSemantic elements
        VES_COUNT
    };

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#if __PSP__
#include "hardwarevertexbufferbase.h"
namespace chrissly
{
namespace graphics
{

typedef HardwareVertexBufferBase HardwareVertexBuffer;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __ANDROID__
#include "hardwarevertexbufferbase.h"
namespace chrissly
{
namespace graphics
{

typedef HardwareVertexBufferBase HardwareVertexBuffer;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "HardwareVertexBuffer class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------
#endif
