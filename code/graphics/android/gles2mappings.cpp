//------------------------------------------------------------------------------
//  gles2mappings.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "gles2mappings.h"
#include "debug.h"
#include <GLES2/gl2ext.h>

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
void
GLES2Mappings::MakeGLMatrix(GLfloat gl_matrix[16], const core::Matrix4& m)
{
    unsigned int x = 0;

    unsigned int i;
    for (i = 0; i < 4; i++)
    {
        unsigned int j;
        for (j = 0; j < 4; j++)
        {
            gl_matrix[x] = m[j][i];
            x++;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
GLbitfield
GLES2Mappings::Get(graphics::FrameBufferType fbt)
{
    GLbitfield clearBits = 0;

    if (fbt & graphics::FBT_COLOUR)  clearBits |= GL_COLOR_BUFFER_BIT;
    if (fbt & graphics::FBT_DEPTH)   clearBits |= GL_DEPTH_BUFFER_BIT;
    if (fbt & graphics::FBT_STENCIL) clearBits |= GL_STENCIL_BUFFER_BIT;
 
    return clearBits;
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2Mappings::Get(unsigned int colour, float& red, float& green, float& blue, float& alpha)
{
    alpha = ((colour & 0xff000000) >> 24) / 255.0f;
    blue  = ((colour & 0x00ff0000) >> 16) / 255.0f;
    green = ((colour & 0x0000ff00) >> 8) / 255.0f;
    red   = ((colour & 0x000000ff)) / 255.0f;
}

//------------------------------------------------------------------------------
/**
*/
GLenum
GLES2Mappings::Get(graphics::PixelFormat pf)
{
    switch (pf)
    {
        case graphics::PF_R5G6B5:        return GL_UNSIGNED_SHORT_5_6_5;
        case graphics::PF_A1R5G5B5:      return GL_UNSIGNED_SHORT_5_5_5_1;
        case graphics::PF_A4R4G4B4:      return GL_UNSIGNED_SHORT_4_4_4_4;
        case graphics::PF_R8G8B8A8:      return GL_UNSIGNED_BYTE;
        case graphics::PF_ETC1_RGB8:     return GL_ETC1_RGB8_OES; 
        case graphics::PF_COLOR_INDEX4:
        case graphics::PF_COLOR_INDEX8:
        case graphics::PF_COLOR_INDEX16:
        case graphics::PF_COLOR_INDEX32:
        case graphics::PF_DXT1:
        case graphics::PF_DXT3:
        case graphics::PF_DXT5:
        default: CE_ASSERT(false, "GLES2Mappings::Get(): illegal or not supported PixelFormat '%i'\n", pf);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
GLint
GLES2Mappings::GetInternalFormat(graphics::PixelFormat pf)
{
    switch (pf)
    {
        case graphics::PF_R5G6B5:        return GL_RGB;
        case graphics::PF_A1R5G5B5:      return GL_RGBA;
        case graphics::PF_A4R4G4B4:      return GL_RGBA;
        case graphics::PF_R8G8B8A8:      return GL_RGBA;
        case graphics::PF_ETC1_RGB8:     return GL_RGB; 
        case graphics::PF_COLOR_INDEX4:
        case graphics::PF_COLOR_INDEX8:
        case graphics::PF_COLOR_INDEX16:
        case graphics::PF_COLOR_INDEX32:
        case graphics::PF_DXT1:
        case graphics::PF_DXT3:
        case graphics::PF_DXT5:
        default: CE_ASSERT(false, "GLES2Mappings::Get(): illegal or not supported PixelFormat '%i'\n", pf);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
GLES2Mappings::IsCompressed(graphics::PixelFormat pf)
{
    switch (pf)
    {
        case graphics::PF_R5G6B5:        return false;
        case graphics::PF_A1R5G5B5:      return false;
        case graphics::PF_A4R4G4B4:      return false;
        case graphics::PF_R8G8B8A8:      return false;
        case graphics::PF_ETC1_RGB8:     return true;
        case graphics::PF_COLOR_INDEX4:
        case graphics::PF_COLOR_INDEX8:
        case graphics::PF_COLOR_INDEX16:
        case graphics::PF_COLOR_INDEX32:
        case graphics::PF_DXT1:
        case graphics::PF_DXT3:
        case graphics::PF_DXT5:
        default: CE_ASSERT(false, "GLES2Mappings::IsCompressed(): illegal or not supported PixelFormat '%i'\n", pf);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
GLenum
GLES2Mappings::Get(graphics::SceneBlendOperation op)
{
    switch (op)
    {
        case graphics::SBO_ADD:              return GL_FUNC_ADD;
        case graphics::SBO_SUBTRACT:         return GL_FUNC_SUBTRACT;
        case graphics::SBO_REVERSE_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
        case graphics::SBO_MIN:
        case graphics::SBO_MAX:
        case graphics::SBO_ABS:
        default: CE_ASSERT(false, "GLES2Mappings::Get(): illegal SceneBlendOperation '%i'\n", op);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
GLenum
GLES2Mappings::Get(graphics::SceneBlendFactor sbf)
{
    switch (sbf)
    {
        case graphics::SBF_ONE:                     return GL_ONE;
        case graphics::SBF_ZERO:                    return GL_ZERO;
        case graphics::SBF_DEST_COLOUR:             return GL_DST_COLOR;
        case graphics::SBF_SOURCE_COLOUR:           return GL_SRC_COLOR;
        case graphics::SBF_ONE_MINUS_DEST_COLOUR:   return GL_ONE_MINUS_DST_COLOR;
        case graphics::SBF_ONE_MINUS_SOURCE_COLOUR: return GL_ONE_MINUS_SRC_COLOR;
        case graphics::SBF_DEST_ALPHA:              return GL_DST_ALPHA;
        case graphics::SBF_SOURCE_ALPHA:            return GL_SRC_ALPHA;
        case graphics::SBF_ONE_MINUS_DEST_ALPHA:    return GL_ONE_MINUS_DST_ALPHA;
        case graphics::SBF_ONE_MINUS_SOURCE_ALPHA:  return GL_ONE_MINUS_SRC_ALPHA;
        case graphics::SBF_FIX: // only PSP
        default: CE_ASSERT(false, "GLES2Mappings::Get(): illegal SceneBlendFactor '%i'\n", sbf);
    }

    return 0;
}

} // namespace chrissly
