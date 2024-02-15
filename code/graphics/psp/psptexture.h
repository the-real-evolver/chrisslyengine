#ifndef PSPTEXTURE_H_
#define PSPTEXTURE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::PSPTexture

    (C) 2012 Christian Bleicher
*/
#include "texturebase.h"

//------------------------------------------------------------------------------
namespace chrissly
{

class PSPTexture : public graphics::TextureBase
{
public:
    /// holds information of mipmap levels
    struct MipmapInfo
    {
        int width;
        int height;
        void* buffer;
    };
    /// default constructor
    PSPTexture();
    /// construct from rendertexture
    PSPTexture(graphics::RenderTexture* const rt);
    /// construct cubemap
    PSPTexture(void* faces[6U]);
    /// destructor
    ~PSPTexture();
    /// create mipmap infos
    void CreateInternalResources();
    /// get mipmap info of the given level
    MipmapInfo* GetMipmapInfo(int level) const;

private:
    MipmapInfo* mipmapInfos;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
