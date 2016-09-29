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
#include "psp/psphardwarevertexbuffer.h"
namespace chrissly
{
namespace graphics
{

typedef PSPHardwareVertexBuffer HardwareVertexBuffer;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __ANDROID__
#include "android/gles2hardwarevertexbuffer.h"
namespace chrissly
{
namespace graphics
{

typedef GLES2HardwareVertexBuffer HardwareVertexBuffer;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __WINDOWS__
#include "windows/d3d11hardwarevertexbuffer.h"
namespace chrissly
{
namespace graphics
{

typedef D3D11HardwareVertexBuffer HardwareVertexBuffer;

} // namespace graphics
} // namespace chrissly
  //------------------------------------------------------------------------------
#else
#error "HardwareVertexBuffer class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------
#endif
