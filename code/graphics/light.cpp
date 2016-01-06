//------------------------------------------------------------------------------
//  light.cpp
//  (C) 2013 Christian Bleicher
//------------------------------------------------------------------------------
#include "light.h"

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
Light::~Light()
{

}

//------------------------------------------------------------------------------
/**
*/
Light::Light() :
    lightType(LT_POINT),
    diffuse(0xffffffff),
    specular(0x00000000),
    range(100.0f),
    attenuationConst(1.0f),
    attenuationLinear(0.0f),
    attenuationQuad(0.0f),
    position(Vector3()),
    direction(Vector3(0.0f, 0.0f, 1.0f)),
    spotOuter(0.78f),
    spotFalloff(1.0f)
{

}

//------------------------------------------------------------------------------
/**
*/
void
Light::SetType(LightTypes type)
{
    this->lightType = type;
}

//------------------------------------------------------------------------------
/**
*/
Light::LightTypes
Light::GetType() const
{
    return this->lightType;
}

//------------------------------------------------------------------------------
/**
*/
void
Light::SetDiffuseColour(unsigned int colour)
{
    this->diffuse = colour;
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
Light::GetDiffuseColour() const
{
    return this->diffuse;
}

//------------------------------------------------------------------------------
/**
*/
void
Light::SetSpecularColour(unsigned int colour)
{
    this->specular = colour;
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
Light::GetSpecularColour() const
{
    return this->specular;
}

//------------------------------------------------------------------------------
/**
*/
void
Light::SetAttenuation(float range, float constant, float linear, float quadratic)
{
    this->range = range;
    this->attenuationConst = constant;
    this->attenuationLinear = linear;
    this->attenuationQuad = quadratic;
}

//------------------------------------------------------------------------------
/**
*/
float
Light::GetAttenuationRange() const
{
    return this->range;
}

//------------------------------------------------------------------------------
/**
*/
float
Light::GetAttenuationConstant() const
{
    return this->attenuationConst;
}

//------------------------------------------------------------------------------
/**
*/
float
Light::GetAttenuationLinear() const
{
    return this->attenuationLinear;
}

//------------------------------------------------------------------------------
/**
*/
float
Light::GetAttenuationQuadric() const
{
    return this->attenuationQuad;
}

//------------------------------------------------------------------------------
/**
*/
void
Light::SetPosition(float x, float y, float z)
{
    this->position.x = x;
    this->position.y = y;
    this->position.z = z;
}

//------------------------------------------------------------------------------
/**
*/
void
Light::SetPosition(const Vector3& pos)
{
    this->position = pos;
}

//------------------------------------------------------------------------------
/**
*/
const Vector3&
Light::GetPosition() const
{
    return this->position;
}

//------------------------------------------------------------------------------
/**
*/
void
Light::SetDirection(float x, float y, float z)
{
    this->direction.x = x;
    this->direction.y = y;
    this->direction.z = z;
}

//------------------------------------------------------------------------------
/**
*/
void
Light::SetDirection(const Vector3& dir)
{
    this->direction = dir;
}

//------------------------------------------------------------------------------
/**
*/
const Vector3&
Light::GetDirection() const
{
    return this->direction;
}

//------------------------------------------------------------------------------
/**
*/
void
Light::SetSpotlightOuterAngle(float angle)
{
    this->spotOuter = angle;
}

//------------------------------------------------------------------------------
/**
*/
float
Light::GetSpotlightOuterAngle() const
{
    return this->spotOuter;
}

//------------------------------------------------------------------------------
/**
*/
void
Light::SetSpotlightFalloff(float falloff)
{
    this->spotFalloff = falloff;
}

//------------------------------------------------------------------------------
/**
*/
float
Light::GetSpotlightFalloff() const
{
    return this->spotFalloff;
}

} // namespace graphics
} // namespace chrissly
