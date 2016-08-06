#ifndef PSPTEXTURE_H_
#define PSPTEXTURE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::PSPTexture

    (C) 2012 Christian Bleicher
*/
#include "texturebase.h"
#include "dynamicarray.h"

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
    /// destructor
    ~PSPTexture();
    /// create mipmap infos
    void CreateInternalResources();
    /// get mipmap info of the given level
    MipmapInfo* GetMipmapInfo(int level);

private:
    mutable ce_dynamic_array mipmapInfos;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
