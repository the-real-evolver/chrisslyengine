#ifndef GLES2MAPPINGS_H_
#define GLES2MAPPINGS_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::GLES2Mappings

    (C) 2012 Christian Bleicher
*/
#include "common.h"
#include "matrix4.h"
#include "pixelformat.h"
#include "blendmode.h"
#include <GLES2/gl2.h>

//------------------------------------------------------------------------------
namespace chrissly 
{
class GLES2Mappings
{
public:
    /// convert matrix to OpenGl ES style
    static void MakeGLMatrix(GLfloat gl_matrix[16], const core::Matrix4& m);
    /// return a OpenGL ES equivalent for a FrameBufferType value
    static GLbitfield Get(graphics::FrameBufferType fbt);
    /// get rgba components of the given color (Format: AABBGGRR) as float value
    static void Get(unsigned int colour, float& red, float& green, float& blue, float& alpha);
    /// return a OpenGL ES equivalent for a PixelFormat value
    static GLenum Get(graphics::PixelFormat pf);
    /// returns the OpenGL ES internal format for a PixelFormat value
    static GLint GetInternalFormat(graphics::PixelFormat pf);
    /// return true if the PixelFormat is a compressed format
    static bool IsCompressed(graphics::PixelFormat pf);
    /// return a OpenGL ES equivalent for a SceneBlendOperation value
    static GLenum Get(graphics::SceneBlendOperation op);
    /// return a OpenGL ES equivalent for a SceneBlendFactor value
    static GLenum Get(graphics::SceneBlendFactor sbf);
};
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
