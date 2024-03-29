#ifndef D3D11TEXTURE_H_
#define D3D11TEXTURE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::D3D11Texture

    (C) 2016 Christian Bleicher
*/
#include "texturebase.h"
#include <d3d11.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class D3D11Texture : public graphics::TextureBase
{
public:
    /// default constructor
    D3D11Texture();
    /// construct from rendertexture
    D3D11Texture(graphics::RenderTexture* const rt);
    /// construct cubemap
    D3D11Texture(void* faces[6U]);
    /// destructor
    ~D3D11Texture();
    /// create d3d11 tetxure and shader resource view
    void CreateInternalResources();

    /// gets a pointer to the shader resource view
    ID3D11ShaderResourceView* GetShaderResourceView() const;

private:
    ID3D11ShaderResourceView* shaderResourceView;
    ID3D11Texture2D* texture;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
