#ifndef PASS_H_
#define PASS_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::Pass

    (C) 2010 Christian Bleicher
*/
#include "common.h"
#include "textureunitstate.h"
#include "blendmode.h"
#include "gpuprogram.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class Pass
{
public:
    /// constructor
    Pass(unsigned short index);
    /// destructor
    ~Pass();

    /// enables/disables blending
    void SetSceneBlendingEnabled(bool enabled);
    /// returns if scene blending is enabled
    bool GetSceneBlendingEnabled() const;
    /// sets the specific operation used to blend source and destination pixels together
    /** 
        @remarks
        SBO_ADD<br>
        SBO_SUBTRACT<br>
        SBO_REVERSE_SUBTRACT<br>
        SBO_MIN<br>
        SBO_MAX<br>
        SBO_ABS (only PSP)<br>
    */
    void SetSceneBlendingOperation(SceneBlendOperation op);
    /// returns the current blending operation 
    SceneBlendOperation GetSceneBlendingOperation() const;
    /// allows very fine control of blending this Pass with the existing contents of the scene
    /**
        @remarks
        SBF_ONE<br>
        SBF_ZERO<br>
        SBF_DEST_COLOUR<br>
        SBF_SOURCE_COLOUR<br>
        SBF_ONE_MINUS_DEST_COLOUR<br>
        SBF_ONE_MINUS_SOURCE_COLOUR<br>
        SBF_DEST_ALPHA<br>
        SBF_SOURCE_ALPHA<br>
        SBF_ONE_MINUS_DEST_ALPHA<br>
        SBF_ONE_MINUS_SOURCE_ALPHA<br>
        SBF_FIX // only PSP<br>
    */
    void SetSceneBlending(const SceneBlendFactor sourceFactor, const SceneBlendFactor destFactor);
    /// retrieves the source blending factor for the material (as set using Material::SetSceneBlending)
    SceneBlendFactor GetSourceBlendFactor() const;
    /// retrieves the destination blending factor for the material (as set using Material::SetSceneBlending)
    SceneBlendFactor GetDestBlendFactor() const;
    /// set fix colors used when GU_FIX operation is applied
    void SetBlendingFixColors(unsigned int srcFixColor, unsigned int dstFixColor);
    /// retrieves the source fix colors
    unsigned int GetSourceBlendingFixColor() const;
    /// retrieves the destination fix colors
    unsigned int GetDestinationBlendingFixColor() const;

    /// sets whether or not this pass renders with depth-buffer checking on or not
    void SetDepthCheckEnabled(bool enabled);
    /// returns whether or not this pass renders with depth-buffer checking on or not
    bool GetDepthCheckEnabled() const;

    /// sets whether or not this pass renders with depth-buffer writing on or not
    /**
        @remarks
        If depth-buffer writing is on, whenever a pixel is written to the frame buffer
        the depth buffer is updated with the depth value of that new pixel, thus affecting future
        rendering operations if future pixels are behind this one.
        @par
        If depth writing is off, pixels are written without updating the depth buffer Depth writing should
        normally be on but can be turned off when rendering static backgrounds or when rendering a collection
        of transparent objects at the end of a scene so that they overlap each other correctly.
    */
    void SetDepthWriteEnabled(bool enabled);
    /// returns whether or not this pass renders with depth-buffer writing on or not
    bool GetDepthWriteEnabled() const;

    /// sets the culling mode for this pass based on the 'vertex winding'
    void SetCullingMode(CullingMode mode);
    /// returns the culling mode for geometry rendered with this pass
    CullingMode GetCullingMode() const;

    /// sets whether or not dynamic lighting is enabled
    void SetLightingEnabled(bool enabled);
    /// returns whether or not dynamic lighting is enabled
    bool GetLightingEnabled() const;
    /// sets the shininess of the pass, affecting the size of specular highlights
    /**
        @note
        This setting has no effect if dynamic lighting is disabled (see Pass::SetLightingEnabled).
    */
    void SetShininess(float val);
    /// gets the 'shininess' property of the pass (affects specular highlights)
    float GetShininess() const;
    /// sets the ambient colour reflectance properties of this pass (Format: 0xBBGGRR)
    /**
        @remarks
        The base colour of a pass is determined by how much red, green and blue light it reflects.
        This property determines how much ambient light (directionless global light) is reflected.
        The default is full white, meaning objects are completely globally illuminated. Reduce
        this if you want to see diffuse or specular light effects, or change the blend of colours
        to make the object have a base colour other than white.
        @note
        This setting has no effect if dynamic lighting is disabled (see Pass::SetLightingEnabled).
    */
    void SetAmbient(unsigned int amb);
    /// gets the ambient colour reflectance of the pass
    unsigned int GetAmbient() const;
    /// sets the diffuse colour reflectance properties of this pass
    /**
        @remarks
        The base colour of a pass is determined by how much red, green and blue light is reflects
        (provided texture layer #0 has a blend mode other than LBO_REPLACE). This property determines how
        much diffuse light (light from instances of the Light class in the scene) is reflected. The default
        is full white, meaning objects reflect the maximum white light they can from Light objects.
        @note
        This setting has no effect if dynamic lighting is disabled (see Pass::SetLightingEnabled).
    */
    void SetDiffuse(unsigned int diff);
    /// gets the diffuse colour reflectance of the pass
    unsigned int GetDiffuse() const;
    /// sets the specular colour reflectance properties of this pass
    /**
        @remarks
        The base colour of a pass is determined by how much red, green and blue light is reflects
        (provided texture layer #0 has a blend mode other than LBO_REPLACE). This property determines how
        much specular light (highlights from instances of the Light class in the scene) is reflected.
        The default is to reflect no specular light.
        @note
        The size of the specular highlights is determined by the separate 'shininess' property.
        @note
        This setting has no effect if dynamic lighting is disabled (see Pass::SetLightingEnabled).
    */
    void SetSpecular(unsigned int spec);
    /// gets the specular colour reflectance of the pass
    unsigned int GetSpecular() const;
    /// sets the amount of self-illumination an object has
    /**
        @remarks
        If an object is self-illuminating, it does not need external sources to light it, ambient or
        otherwise. It's like the object has it's own personal ambient light. This property is rarely useful since
        you can already specify per-pass ambient light, but is here for completeness.
        @note
        This setting has no effect if dynamic lighting is disabled (see Pass::SetLightingEnabled).
    */
    void SetSelfIllumination(unsigned int selfIllum);
    ///  gets the self illumination colour of the pass
    unsigned int GetSelfIllumination() const;
    /// gets which material properties follow the vertex colour
    unsigned int GetVertexColourTracking() const;
    /// sets which material properties follow the vertex colour
    void SetVertexColourTracking(unsigned int track);

    /// gets the index of this Pass in the parent Material
    unsigned short GetIndex () const;
    
    /// sets the fogging mode applied to this pass
    /**
        @param
        mode You can disable fog which is turned on for the rest of the scene
        by specifying FOG_NONE. Otherwise, set a pass-specific fog mode as
        defined in the enum FogMode.
        @param
        colour The colour of the fog.
        @param
        linearStart Distance in world units at which linear fog starts to encroach.
        Only applicable if mode is FOG_LINEAR.
        @param
        linearEnd Distance in world units at which linear fog becomes completely opaque.
        Only applicable if mode is FOG_LINEAR.
    */
    void SetFog(FogMode mode = FOG_NONE, unsigned int colour = 0xffffffff, float linearStart = 0.0f, float linearEnd = 1.0f);
    /// Returns the fog mode for this pass.
    FogMode GetFogMode() const;
    /// returns the fog colour for the scene
    unsigned int GetFogColour() const;
    /// returns the fog start distance for this pass
    float GetFogStart() const;
    /// returns the fog end distance for this pass
    float GetFogEnd() const;

    /// sets whether the attached or the default gpu-program includes the required instructions to perform morph animation
    void SetMorphAnimationIncluded(bool included);
    /// returns whether the attached or the default gpu-program includes the required instructions to perform morph animation
    bool IsMorphAnimationIncluded() const;

    /// inserts a new TextureUnitState object into the Pass
    TextureUnitState* const CreateTextureUnitState();
    /// retrieves a pointer to a texture unit state so it may be modified
    TextureUnitState* const GetTextureUnitState(unsigned short i) const;
    /// returns the number of texture unit settings
    unsigned short GetNumTextureUnitStates() const;
    /// Removes all texture unit settings
    void RemoveAllTextureUnitStates();

    /// returns true if this pass is programmable i.e. includes a gpu program
    bool IsProgrammable() const;
    /// sets the gpu program to use
    void SetGpuProgram(GpuProgram* const program);
    /// gets the gpu program used by this pass
    GpuProgram* const GetGpuProgram() const;

    /// internal method for loading this pass
    void _Load();

private:
    SceneBlendOperation blendOperation;
    SceneBlendFactor sourceBlendFactor;
    SceneBlendFactor destBlendFactor;
    bool sceneBlend;
    unsigned int sourceFixColor;
    unsigned int destFixColor;
    bool depthCheck;
    bool depthWrite;
    CullingMode cullMode;
    bool lightingEnabled;
    unsigned int ambient;
    unsigned int diffuse;
    unsigned int specular;
    unsigned int emissive;
    float shininess;
    unsigned int tracking;
    unsigned short index;
    FogMode fogMode;
    unsigned int fogColour;
    float fogStart;
    float fogEnd;
    bool morphAnimation;

    TextureUnitState** textureUnitStates;

    GpuProgram* gpuProgram;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
