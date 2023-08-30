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
GLES2Mappings::MakeGLMatrix(GLfloat gl_matrix[16U], const core::Matrix4& m)
{
    gl_matrix[0U]  = m[0U][0U]; gl_matrix[1U]  = m[1U][0U]; gl_matrix[2U]  = m[2U][0U]; gl_matrix[3U]  = m[3U][0U];
    gl_matrix[4U]  = m[0U][1U]; gl_matrix[5U]  = m[1U][1U]; gl_matrix[6U]  = m[2U][1U]; gl_matrix[7U]  = m[3U][1U];
    gl_matrix[8U]  = m[0U][2U]; gl_matrix[9U]  = m[1U][2U]; gl_matrix[10U] = m[2U][2U]; gl_matrix[11U] = m[3U][2U];
    gl_matrix[12U] = m[0U][3U]; gl_matrix[13U] = m[1U][3U]; gl_matrix[14U] = m[2U][3U]; gl_matrix[15U] = m[3U][3U];
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

    return 0U;
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

    return 0U;
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2Mappings::Get(graphics::FilterOptions minFilter, graphics::FilterOptions magFilter, graphics::FilterOptions mipFilter, GLint& min, GLint& mag)
{
    switch (mipFilter)
    {
        case graphics::FO_NONE:
            {
                switch (minFilter)
                {
                    case graphics::FO_NONE:   min = GL_NEAREST; break;
                    case graphics::FO_POINT:  min = GL_NEAREST; break;
                    case graphics::FO_LINEAR: min = GL_LINEAR;  break;
                    default: CE_ASSERT(false, "GLES2Mappings::Get(): illegal FilterOptions '%i'\n", minFilter);
                }
            }
            break;

        case graphics::FO_POINT:
            {
                switch (minFilter)
                {
                    case graphics::FO_NONE:   min = GL_NEAREST_MIPMAP_NEAREST; break;
                    case graphics::FO_POINT:  min = GL_NEAREST_MIPMAP_NEAREST; break;
                    case graphics::FO_LINEAR: min = GL_LINEAR_MIPMAP_NEAREST;  break;
                    default: CE_ASSERT(false, "GLES2Mappings::Get(): illegal FilterOptions '%i'\n", minFilter);
                }
            }
            break;

        case graphics::FO_LINEAR:
            {
                switch (minFilter)
                {
                    case graphics::FO_NONE:   min = GL_NEAREST_MIPMAP_LINEAR; break;
                    case graphics::FO_POINT:  min = GL_NEAREST_MIPMAP_LINEAR; break;
                    case graphics::FO_LINEAR: min = GL_LINEAR_MIPMAP_LINEAR;  break;
                    default: CE_ASSERT(false, "GLES2Mappings::Get(): illegal FilterOptions '%i'\n", minFilter);
                }
            }
            break;

        default: CE_ASSERT(false, "GLES2Mappings::Get(): illegal FilterOptions '%i'\n", mipFilter);
    }

    switch (magFilter)
    {
        case graphics::FO_NONE:   mag = GL_NEAREST; break;
        case graphics::FO_POINT:  mag = GL_NEAREST; break;
        case graphics::FO_LINEAR: mag = GL_LINEAR;  break;
        default: CE_ASSERT(false, "GLES2Mappings::Get(): illegal FilterOptions '%i'\n", magFilter);
    }
}

//------------------------------------------------------------------------------
/**
*/
GLint
GLES2Mappings::Get(graphics::TextureUnitState::TextureAddressingMode tam)
{
    switch (tam)
    {
        case graphics::TextureUnitState::TAM_WRAP:   return GL_REPEAT;
        case graphics::TextureUnitState::TAM_MIRROR: return GL_MIRRORED_REPEAT;
        case graphics::TextureUnitState::TAM_CLAMP:  return GL_CLAMP_TO_EDGE;
        case graphics::TextureUnitState::TAM_BORDER: return GL_CLAMP_TO_EDGE;
        default: CE_ASSERT(false, "GLES2Mappings::Get(): illegal TextureAddressingMode '%i'\n", tam);
    }

    return 0;
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
        case graphics::PF_R8G8B8:        return GL_UNSIGNED_BYTE;
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

    return 0U;
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
        case graphics::PF_R8G8B8:        return GL_RGB;
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
        default: CE_ASSERT(false, "GLES2Mappings::GetInternalFormat(): illegal or not supported PixelFormat '%i'\n", pf);
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
        case graphics::PF_R8G8B8:        return false;
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

    return false;
}

//------------------------------------------------------------------------------
/**
*/
GLsizei
GLES2Mappings::GetImageSize(graphics::PixelFormat pf, GLsizei width, GLsizei height)
{
    switch (pf)
    {
        // ETC1 provides 6x compression of 24-bit RGB
        case graphics::PF_ETC1_RGB8: return (unsigned int)(width * height) >> 1U;
        default: CE_ASSERT(false, "GLES2Mappings::GetImageSize(): illegal or not supported PixelFormat '%i'\n", pf);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
GLbitfield
GLES2Mappings::Get(graphics::FrameBufferType fbt)
{
    GLbitfield clearBits = 0U;

    if (fbt & graphics::FBT_COLOUR)  {clearBits |= GL_COLOR_BUFFER_BIT;}
    if (fbt & graphics::FBT_DEPTH)   {clearBits |= GL_DEPTH_BUFFER_BIT;}
    if (fbt & graphics::FBT_STENCIL) {clearBits |= GL_STENCIL_BUFFER_BIT;}

    return clearBits;
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2Mappings::Get(unsigned int colour, float& red, float& green, float& blue, float& alpha)
{
    alpha = ((colour & 0xff000000) >> 24U) / 255.0f;
    blue  = ((colour & 0x00ff0000) >> 16U) / 255.0f;
    green = ((colour & 0x0000ff00) >> 8U) / 255.0f;
    red   = (colour & 0x000000ff) / 255.0f;
}

//------------------------------------------------------------------------------
/**
*/
GLenum
GLES2Mappings::Get(graphics::TextureType tt)
{
    switch (tt)
    {
        case graphics::TEX_TYPE_2D:         return GL_TEXTURE_2D;
        case graphics::TEX_TYPE_CUBE_MAP:   return GL_TEXTURE_CUBE_MAP;
        default: CE_ASSERT(false, "GLES2Mappings::Get(): illegal or not supported TextureType '%i'\n", tt);
    }

    return 0U;
}

} // namespace chrissly
