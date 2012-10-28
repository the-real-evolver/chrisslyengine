//------------------------------------------------------------------------------
//  pass.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "pass.h"

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
    cullMode(CULL_ANTICLOCKWISE),
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
    numTextureUnitStates(0)
{
    DynamicArrayInit(&this->textureUnitStates, 0);
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
Pass::SetBlendingFixColors(unsigned int sourceFixColor, unsigned int destFixColor)
{
    this->sourceFixColor = sourceFixColor;
    this->destFixColor = destFixColor;
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
Pass::SetAmbient(unsigned int ambient)
{
    this->ambient = ambient;
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
Pass::SetDiffuse(unsigned int diffuse)
{
    this->diffuse = diffuse;
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
Pass::SetSpecular(unsigned int specular)
{
    this->specular = specular;
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
Pass::SetVertexColourTracking(unsigned int tracking)
{
    this->tracking = tracking;
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
TextureUnitState*
Pass::CreateTextureUnitState()
{
    TextureUnitState* textureUnitState = CE_NEW TextureUnitState();
    
    if (!DynamicArraySet(&this->textureUnitStates, this->numTextureUnitStates, textureUnitState))
    {
        return NULL;
    }
    
    this->numTextureUnitStates++;

    return textureUnitState;
}

//------------------------------------------------------------------------------
/**
*/
TextureUnitState*
Pass::GetTextureUnitState(unsigned short index) const
{
    return (TextureUnitState*)DynamicArrayGet(&this->textureUnitStates, index);
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
Pass::GetNumTextureUnitStates() const
{
    return this->numTextureUnitStates;
}

//------------------------------------------------------------------------------
/**
*/
void
Pass::RemoveAllTextureUnitStates()
{
    unsigned int i;
    for (i = 0; i < this->numTextureUnitStates; i++)
    {
        CE_DELETE (TextureUnitState*)DynamicArrayGet(&this->textureUnitStates, i);	
    }

    DynamicArrayDelete(&this->textureUnitStates);
    
    this->numTextureUnitStates = 0;
}

} // namespace graphics
} // namespace chrissly
