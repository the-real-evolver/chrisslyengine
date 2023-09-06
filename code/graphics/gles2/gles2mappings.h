#ifndef GLES2MAPPINGS_H_
#define GLES2MAPPINGS_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::GLES2Mappings

    (C) 2012 Christian Bleicher
*/
#include "matrix4.h"
#include "blendmode.h"
#include "pixelformat.h"
#include "common.h"
#include "textureunitstate.h"
#include <GLES2/gl2.h>

//------------------------------------------------------------------------------
namespace chrissly
{
class GLES2Mappings
{
public:
    /// convert matrix to OpenGl ES style
    static void MakeGLMatrix(GLfloat gl_matrix[16U], const core::Matrix4& m);
    /// return a OpenGL ES equivalent for a SceneBlendOperation value
    static GLenum Get(graphics::SceneBlendOperation op);
    /// return a OpenGL ES equivalent for a SceneBlendFactor value
    static GLenum Get(graphics::SceneBlendFactor sbf);
    /// return a OpenGL ES equivalent for a FilterOptions value
    static void Get(graphics::FilterOptions minFilter, graphics::FilterOptions magFilter, graphics::FilterOptions mipFilter, GLint& min, GLint& mag);
    /// return a OpenGL ES equivalent for a TextureAddressingMode value
    static GLint Get(graphics::TextureUnitState::TextureAddressingMode tam);
    /// return a OpenGL ES equivalent for a PixelFormat value
    static GLenum Get(graphics::PixelFormat pf);
    /// returns the OpenGL ES internal format for a PixelFormat value
    static GLint GetInternalFormat(graphics::PixelFormat pf);
    /// return true if the PixelFormat is a compressed format
    static bool IsCompressed(graphics::PixelFormat pf);
    /// return the byte size of an compressed image with the given format and dimensions
    static GLsizei GetImageSize(graphics::PixelFormat pf, GLsizei width, GLsizei height);
    /// return a OpenGL ES equivalent for a FrameBufferType value
    static GLbitfield Get(graphics::FrameBufferType fbt);
    /// return a OpenGL ES equivalent for a TextureType value
    static GLenum Get(graphics::TextureType tt);
};
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
