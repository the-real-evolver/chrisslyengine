#ifndef COMMON_H_
#define COMMON_H_
//------------------------------------------------------------------------------
/**
    Common stuff like enumerations

    (C) 2011 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{
    /**
        hardware culling modes based on vertex winding. This setting applies to
        how the hardware API culls triangles it is sent
    */
    enum CullingMode
    {
        /// Hardware never culls triangles and renders everything it receives.
        CULL_NONE,
        /// Hardware culls triangles whose vertices are listed clockwise in the view (default).
        CULL_CLOCKWISE,
        /// Hardware culls triangles whose vertices are listed anticlockwise in the view.
        CULL_ANTICLOCKWISE
    };

    /// filter type
    enum FilterType
    {
        /// The filter used when shrinking a texture
        FT_MIN,
        /// The filter used when magnifying a texture
        FT_MAG,
        /// The filter used when determining the mipmap
        FT_MIP
    };

    /// filtering options for textures / mipmaps
    enum FilterOptions
    {
        /// No filtering, used for FILT_MIP to turn off mipmapping
        FO_NONE,
        /// Use the closest pixel
        FO_POINT,
        /// Average of a 2x2 pixel area, denotes bilinear for MIN and MAG, trilinear for MIP
        FO_LINEAR
    };

    /// an enumeration of broad shadow techniques
    enum ShadowTechnique
    {
        /// No shadows
        SHADOWTYPE_NONE,
        /**
            Texture-based shadow technique which involves a render-to-texture
            of the shadow caster and a projection of that texture onto the
            shadow receivers, built up per light as additive passes. This
            technique can be very fillrate intensive because it requires
            numLights + 2 passes of the entire scene. However, it is a more
            accurate model than the modulative approach and this is especially
            apparent when using coloured lights or bump mapping.
        */
        SHADOWTYPE_TEXTURE_ADDITIVE
    };

    /// defines the frame buffer types
    enum FrameBufferType
    {
        FBT_COLOUR     = 0x1,
        FBT_DEPTH      = 0x2,
        FBT_STENCIL    = 0x4,
        FBT_FAST_CLEAR = 0x10 // only PSP
    };

    /// an enumeration describing which material properties should track the vertex colours
    enum TrackVertexColourType
    {
        TVC_NONE        = 0x0,
        TVC_AMBIENT     = 0x1,
        TVC_DIFFUSE     = 0x2,
        TVC_SPECULAR    = 0x4,
        TVC_EMISSIVE    = 0x8
    };

    /// fog modes
    enum FogMode
    {
        /// disable fog
        FOG_NONE = 0,
        /// fog density increases linearly between the start and end distances
        FOG_LINEAR
    };

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif