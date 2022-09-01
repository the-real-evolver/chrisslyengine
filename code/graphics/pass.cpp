//------------------------------------------------------------------------------
//  pass.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "pass.h"
#include "chrisslyarray.h"

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
Pass::Pass(unsigned short index) :
    blendOperation(SBO_ADD),
    sourceBlendFactor(SBF_SOURCE_COLOUR),
    destBlendFactor(SBF_SOURCE_COLOUR),
    sceneBlend(false),
    sourceFixColor(0xff666666),
    destFixColor(0xff000000),
    depthCheck(true),
    depthWrite(true),
    cullMode(CULL_CLOCKWISE),
    lightingEnabled(false),
    ambient(0xffffffff),
    diffuse(0xffffffff),
    specular(0x00000000),
    emissive(0x00000000),
    shininess(0.0f),
    tracking(TVC_NONE),
    index(index),
    fogMode(FOG_NONE),
    fogColour(0xffffffff),
    fogStart(0.0f),
    fogEnd(1.0f),
    morphAnimation(false),
    skeletalAnimation(false),
    textureUnitStates(NULL),
    gpuProgram(NULL)
{
    ce_array_init(this->textureUnitStates, 1U);
}

//------------------------------------------------------------------------------
/**
*/
Pass::~Pass()
{
    this->RemoveAllTextureUnitStates();
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetSceneBlendingEnabled(bool enabled)
{
    this->sceneBlend = enabled;
}

//------------------------------------------------------------------------------
/**
*/
bool
Pass::GetSceneBlendingEnabled() const
{
    return this->sceneBlend;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetSceneBlendingOperation(SceneBlendOperation op)
{
    this->blendOperation = op;
}

//------------------------------------------------------------------------------
/**
*/
SceneBlendOperation
Pass::GetSceneBlendingOperation() const
{
    return this->blendOperation;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetSceneBlending(const SceneBlendFactor sourceFactor, const SceneBlendFactor destFactor)
{
    this->sourceBlendFactor = sourceFactor;
    this->destBlendFactor = destFactor;
}

//------------------------------------------------------------------------------
/**
*/
SceneBlendFactor
Pass::GetSourceBlendFactor() const
{
    return this->sourceBlendFactor;
}

//------------------------------------------------------------------------------
/**
*/
SceneBlendFactor
Pass::GetDestBlendFactor() const
{
    return this->destBlendFactor;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetBlendingFixColors(unsigned int srcFixColor, unsigned int dstFixColor)
{
    this->sourceFixColor = srcFixColor;
    this->destFixColor = dstFixColor;
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
Pass::GetSourceBlendingFixColor() const
{
    return this->sourceFixColor;
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
Pass::GetDestinationBlendingFixColor() const
{
    return this->destFixColor;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetDepthCheckEnabled(bool enabled)
{
    this->depthCheck = enabled;
}

//------------------------------------------------------------------------------
/**
*/
bool
Pass::GetDepthCheckEnabled() const
{
    return this->depthCheck;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetDepthWriteEnabled(bool enabled)
{
    this->depthWrite = enabled;
}

//------------------------------------------------------------------------------
/**
*/
bool
Pass::GetDepthWriteEnabled() const
{
    return this->depthWrite;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetCullingMode(CullingMode mode)
{
    this->cullMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
CullingMode
Pass::GetCullingMode() const
{
    return this->cullMode;
} 

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetLightingEnabled(bool enabled)
{
    this->lightingEnabled = enabled;
}
//------------------------------------------------------------------------------
/**
*/
bool
Pass::GetLightingEnabled() const
{
    return this->lightingEnabled;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetShininess(float val)
{
    this->shininess = val;
}

//------------------------------------------------------------------------------
/**
*/
float
Pass::GetShininess() const
{
    return this->shininess;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetAmbient(unsigned int amb)
{
    this->ambient = amb;
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
Pass::GetAmbient() const
{
    return this->ambient;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetDiffuse(unsigned int diff)
{
    this->diffuse = diff;
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
Pass::GetDiffuse() const
{
    return this->diffuse;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetSpecular(unsigned int spec)
{
    this->specular = spec;
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
Pass::GetSpecular() const
{
    return this->specular;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetSelfIllumination(unsigned int selfIllum)
{
    this->emissive = selfIllum;
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
Pass::GetSelfIllumination() const
{
    return this->emissive;
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
Pass::GetVertexColourTracking() const
{
    return this->tracking;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetVertexColourTracking(unsigned int track)
{
    this->tracking = track;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
Pass::GetIndex() const
{
    return this->index;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetFog(FogMode mode, unsigned int colour, float linearStart, float linearEnd)
{
    this->fogMode = mode;
    this->fogColour = colour;
    this->fogStart = linearStart;
    this->fogEnd = linearEnd;
}

//------------------------------------------------------------------------------
/**
*/
FogMode
Pass::GetFogMode() const
{
    return this->fogMode;
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
Pass::GetFogColour() const
{
    return this->fogColour;
}

//------------------------------------------------------------------------------
/**
*/
float
Pass::GetFogStart() const
{
    return this->fogStart;
}

//------------------------------------------------------------------------------
/**
*/
float
Pass::GetFogEnd() const
{
    return this->fogEnd;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetMorphAnimationIncluded(bool included)
{
    this->morphAnimation = included;
}

//------------------------------------------------------------------------------
/**
*/
bool
Pass::IsMorphAnimationIncluded() const
{
    return this->morphAnimation;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetSkeletalAnimationIncluded(bool included)
{
    this->skeletalAnimation = included;
}

//------------------------------------------------------------------------------
/**
*/
bool
Pass::IsSkeletalAnimationIncluded() const
{
    return this->skeletalAnimation;
}

//------------------------------------------------------------------------------
/**
*/
TextureUnitState* const
Pass::CreateTextureUnitState()
{
    TextureUnitState* textureUnitState = CE_NEW TextureUnitState();
    ce_array_push_back(this->textureUnitStates, textureUnitState);

    return textureUnitState;
}

//------------------------------------------------------------------------------
/**
*/
TextureUnitState* const
Pass::GetTextureUnitState(unsigned short i) const
{
    return this->textureUnitStates[i];
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
Pass::GetNumTextureUnitStates() const
{
    return (unsigned short)ce_array_size(this->textureUnitStates);
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::RemoveAllTextureUnitStates()
{
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->textureUnitStates); ++i)
    {
        CE_DELETE this->textureUnitStates[i];
    }
    ce_array_delete(this->textureUnitStates);
}

//------------------------------------------------------------------------------
/**
*/
bool
Pass::IsProgrammable() const
{
    return (this->gpuProgram != NULL);
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::SetGpuProgram(GpuProgram* const program)
{
    this->gpuProgram = program;
}

//------------------------------------------------------------------------------
/**
*/
GpuProgram* const
Pass::GetGpuProgram() const
{
    return this->gpuProgram;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::_Load()
{
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->textureUnitStates); ++i)
    {
        this->textureUnitStates[i]->_Load();
    }
}

} // namespace graphics
} // namespace chrissly
