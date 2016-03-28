#ifndef TEXTUREUNITSTATE_H_
#define TEXTUREUNITSTATE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::TextureUnitState

    (C) 2012 Christian Bleicher
*/
#include "common.h"
#include "texture.h"
#include "blendmode.h"
#include "chrisslystring.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class TextureUnitState
{
public:
    /// texture addressing modes - default is TAM_WRAP
    enum TextureAddressingMode
    {
        /// Texture wraps at values over 1.0
        TAM_WRAP,
        /// Texture mirrors (flips) at joins over 1.0
        TAM_MIRROR,
        /// Texture clamps at 1.0
        TAM_CLAMP,
        /// Texture coordinates outside the range [0.0, 1.0] are set to the border colour
        TAM_BORDER
    };

    /// texture addressing mode for each texture coordinate
    struct UVWAddressingMode
    {
        TextureAddressingMode u, v;
    };

    /// texture mapping mode - default is TMM_TEXTURE_COORDS
    enum TextureMappingMode
    {
        TMM_TEXTURE_COORDS,
        TMM_TEXTURE_MATRIX,
        TMM_ENVIRONMENT_MAP
    };

    /// texture projection mapping mode - default is TPM_UV
    enum TextureProjectionMappingMode
    {
        TPM_POSITION,
        TPM_UV,
        TPM_NORMALIZED_NORMAL,
        TPM_NORMAL
    };

    /// constructor
    TextureUnitState();
    /// destructor
    ~TextureUnitState();

    /// sets the scaling factor applied to texture coordinates
    void SetTextureScale(float uScale, float vScale);
    /// get texture uscale value
    float GetTextureUScale() const;
    /// get texture vscale value
    float GetTextureVScale() const;
    /// sets an offset added to the texture coordinates
    void SetTextureScroll(float u, float v);
    /// get texture uscroll value
    float GetTextureUScroll() const;
    /// get texture vscroll value
    float GetTextureVScroll() const;
    /// set the detailed filtering options on this texture unit
    /**
        @remarks
        FO_NONE<br>
        FO_POINT<br>
        FO_LINEAR<br>
    */
    void SetTextureFiltering(FilterOptions minFilter, FilterOptions magFilter, FilterOptions mipFilter); 
    /// get the texture filtering for the given type
    FilterOptions GetTextureFiltering(FilterType ftype) const;

    /// Determines how the texture layer is combined with the the diffuse colour of the geometry below it.
    /**
        @remarks
        LBT_COLOUR <br>
        LBT_ALPHA <br>

        LBO_REPLACE <br>
        LBO_ADD <br>
        LBO_MODULATE <br>
        LBO_ALPHA_BLEND <br>
        LBO_DECAL <br>
    */
    void SetTextureBlendOperation(LayerBlendType lbt, LayerBlendOperation lbo);
    /// get the texture operation
    void GetTextureBlendOperation(LayerBlendType& lbt, LayerBlendOperation& lbo) const;

    /// sets the texture addressing mode, i.e. what happens at uv values above 1.0.
    /**
        @note
        The default is TAM_WRAP i.e. the texture repeats over values of 1.0.
    */
    void SetTextureAddressingMode(TextureAddressingMode u, TextureAddressingMode v);
    /// gets the texture addressing mode for a given coordinate i.e. what happens at uv values above 1.0.
    /**    
        @note
        The default is TAM_WRAP i.e. the texture repeats over values of 1.0.
    */
    const UVWAddressingMode& GetTextureAddressingMode() const; 

    /// set the texturemapping mode
    /**
        @remarks
        TMM_TEXTURE_COORDS <br>
        TMM_TEXTURE_MATRIX <br>
        TMM_ENVIRONMENT_MAP <br>
    */
    void SetTextureMappingMode(TextureMappingMode mode);
    /// get the texturemapping mode
    TextureMappingMode GetTextureMappingMode() const;

    /// set the texture projection mapping mode
    /**
        @remarks
        TPM_POSITION <br>
        TPM_UV <br>
        TPM_NORMALIZED_NORMAL <br>
        TPM_NORMAL <br>
    */
    void SetTextureProjectionMappingMode(TextureProjectionMappingMode mode);
    /// get the texture projection mapping mode
    TextureProjectionMappingMode GetTextureProjectionMappingMode() const;

    /// set the name of the texture to use
    void SetTextureName(const char* name);
    /// gets the name of the texture to use
    const core::String& GetTextureName() const;

    /// set texture to use
    void SetTexture(Texture* texture);
    /// get the used texture
    Texture* GetTexture() const;

    /// internal method for loading this object as part of Material::load
    void _Load();

private:
    float uScale;
    float vScale;
    float uMod;
    float vMod;
    FilterOptions minFilter;
    FilterOptions magFilter;
    FilterOptions mipFilter;
    LayerBlendOperation lbo;
    LayerBlendType lbt;
    UVWAddressingMode addressMode;
    TextureMappingMode mappingMode;
    TextureProjectionMappingMode projectionMappingMode;
    core::String textureName;
    Texture* texture;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif