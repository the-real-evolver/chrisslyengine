#ifndef GPUPROGRAMPARAMS_H_
#define GPUPROGRAMPARAMS_H_
//------------------------------------------------------------------------------
/**
    Enumeration of the types of constant we may encounter in programs

    (C) 2012 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

    enum AutoConstantType
    {
        /// the current world matrix
        ACT_WORLD_MATRIX = 0,
        /// the current view matrix
        ACT_VIEW_MATRIX,
        /// the current projection matrix
        ACT_PROJECTION_MATRIX,
        /// the current world, view & projection matrices concatenated
        ACT_WORLDVIEWPROJ_MATRIX,
        /// provides the current transform matrix of the texture unit
        ACT_TEXTURE_MATRIX,
        /// specifies the morph weight when blending between two vertices
        ACT_MORPH_WEIGHT,
        /// the  number of AutoConstantType elements
        ACT_COUNT
    };

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif