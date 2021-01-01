#ifndef GLES2TEXTURE_H_
#define GLES2TEXTURE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::GLES2Texture

    (C) 2012 Christian Bleicher
*/
#include "texturebase.h"
#include <GLES2/gl2.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class GLES2Texture : public graphics::TextureBase
{
public:
    /// default constructor
    GLES2Texture();
    /// construct from rendertexture
    GLES2Texture(graphics::RenderTexture* const rt);
    /// destructor
    ~GLES2Texture();
    /// generate gl texture name
    void CreateInternalResources();
    /// return gl texture name
    GLuint GetName() const;

private:
    GLuint textureName;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
