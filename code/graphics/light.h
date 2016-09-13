#ifndef LIGHT_H_
#define LIGHT_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::Light

    (C) 2013 Christian Bleicher
*/
#include "vector3.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class Light
{
    friend class SceneManager;
public:
    /// defines the type of light
    enum LightTypes
    {
        /// point light sources give off light equally in all directions, so require only position not direction
        LT_POINT = 0,
        /// directional lights simulate parallel light beams from a distant source, hence have direction but no position
        LT_DIRECTIONAL = 1,
        /// spotlights simulate a cone of light from a source so require position and direction, plus extra values for falloff
        LT_SPOTLIGHT = 2
    };

    /// destructor
    ~Light();
    /// sets the type of light - see LightTypes for more info
    void SetType(LightTypes type);
    /// returns the light type
    LightTypes GetType() const;

    /// sets the colour of the diffuse light given off by this source
    /**
        @remarks
            Material objects have ambient, diffuse and specular values which indicate how much of each type of
            light an object reflects. This value denotes the amount and colour of this type of light the light
            exudes into the scene. The actual appearance of objects is a combination of the two.
        @par
            Diffuse light simulates the typical light emanating from light sources and affects the base colour
            of objects together with ambient light.
    */
    void SetDiffuseColour(unsigned int colour);
    /// returns the colour of the diffuse light given off by this light source (see setDiffuseColour for more info)
    unsigned int GetDiffuseColour() const;
    /// sets the colour of the specular light given off by this source
    /**
        @remarks
            Material objects have ambient, diffuse and specular values which indicate how much of each type of
            light an object reflects. This value denotes the amount and colour of this type of light the light
            exudes into the scene. The actual appearance of objects is a combination of the two.
        @par
            Specular light affects the appearance of shiny highlights on objects, and is also dependent on the
            'shininess' Material value.
    */
    void SetSpecularColour(unsigned int colour);
    /// returns the colour of specular light given off by this light source
    unsigned int GetSpecularColour() const;

    /// sets the attenuation parameters of the light source i.e. how it diminishes with distance
    /**
        @remarks
            Lights normally get fainter the further they are away.
        @par
            Light attenuation is not applicable to directional lights since they have an infinite range and
            constant intensity.
        @param
            range The absolute upper range of the light in world units
        @param
            constant The constant factor in the attenuation formula: 1.0 means never attenuate, 0.0 is complete attenuation
        @param
            linear The linear factor in the attenuation formula: 1 means attenuate evenly over the distance
        @param
            quadratic The quadratic factor in the attenuation formula: adds a curvature to the attenuation formula
    */
    void SetAttenuation(float r, float constant, float linear, float quadratic);
    /// returns the absolute upper range of the light
    float GetAttenuationRange() const;
    /// returns the constant factor in the attenuation formula
    float GetAttenuationConstant() const;
    /// returns the linear factor in the attenuation formula
    float GetAttenuationLinear() const;
    /// returns the quadric factor in the attenuation formula
    float GetAttenuationQuadric() const;

    /// sets the position of the light
    /**
        @remarks
            Applicable to point lights and spotlights only.
    */
    void SetPosition(float x, float y, float z);
    /// sets the position of the light
    /**
        @remarks
            Applicable to point lights and spotlights only.
    */
    void SetPosition(const core::Vector3& pos);
    /// returns the position of the light
    /**
        @remarks
            Applicable to point lights and spotlights only.
    */
    const core::Vector3& GetPosition() const;
    /// sets the direction in which a light points
    /**
        @remarks
            Applicable only to the spotlight and directional light types.
    */
    void SetDirection(float x, float y, float z);
    /// sets the direction in which a light points
    /**
        @remarks
            Applicable only to the spotlight and directional light types.
    */
    void SetDirection(const core::Vector3& dir);
    /// returns the light's direction
    /**
        @remarks
            Applicable only to the spotlight and directional light types.
    */
    const core::Vector3& GetDirection() const;

    /// sets the angle covered by the spotlights outer cone
    void SetSpotlightOuterAngle(float angle);
    /// returns the angle covered by the spotlights outer cone
    float GetSpotlightOuterAngle() const;
    /// sets the falloff between the inner and outer cones of the spotlight
    void SetSpotlightFalloff(float falloff);
    /// returns the falloff between the inner and outer cones of the spotlight
    float GetSpotlightFalloff() const;

private:
    /// constructor, called by SceneManager::CreateLight()
    Light();

    LightTypes lightType;
    unsigned int diffuse;
    unsigned int specular;
    float range;
    float attenuationConst;
    float attenuationLinear;
    float attenuationQuad;
    core::Vector3 position;
    core::Vector3 direction;
    float spotOuter;
    float spotFalloff;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
