#ifndef PSPMAPPINGS_H_
#define PSPMAPPINGS_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::PSPMappings

    (C) 2012 Christian Bleicher
*/
#include "matrix4.h"
#include "blendmode.h"
#include "pixelformat.h"
#include "common.h"
#include "textureunitstate.h"
#include "light.h"
#include <psptypes.h>

//------------------------------------------------------------------------------
namespace chrissly
{
class PSPMappings
{
public:
    /// convert matrix to PSP style
    static ScePspFMatrix4 MakePSPMatrix(const core::Matrix4& mat);
    /// return a PSP equivalent for a SceneBlendOperation value
    static int Get(graphics::SceneBlendOperation op);
    /// return a PSP equivalent for a SceneBlendFactor value
    static int Get(graphics::SceneBlendFactor sbf);
    /// return a PSP equivalent for a LayerBlendType value
    static int Get(graphics::LayerBlendType lbt);
    /// return a PSP equivalent for a LayerBlendOperation value
    static int Get(graphics::LayerBlendOperation lbo);
    /// return a PSP equivalent for a FilterOptions value
    static void Get(graphics::FilterOptions minFilter, graphics::FilterOptions magFilter, graphics::FilterOptions mipFilter, int& min, int& mag);
    /// return a PSP equivalent for a TextureAddressingMode value
    static int Get(graphics::TextureUnitState::TextureAddressingMode tam);
    /// return a PSP equivalent for a TextureMappingMode value
    static int Get(graphics::TextureUnitState::TextureMappingMode tmm);
    /// return a PSP equivalent for a TextureProjectionMappingMode value
    static int Get(graphics::TextureUnitState::TextureProjectionMappingMode tpm);
    /// return a PSP equivalent for a PixelFormat value
    static int Get(graphics::PixelFormat pf);
    /// return a PSP equivalent for a FrameBufferType value
    static int Get(graphics::FrameBufferType fbt);
    /// return a PSP equivalent for a TrackVertexColourType value
    static int Get(graphics::TrackVertexColourType tvct);
    /// return a PSP equivalent for a LightType
    static int Get(graphics::Light::LightTypes lt);
};
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
