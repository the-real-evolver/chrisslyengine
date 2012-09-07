#ifndef BLENDMODE_H_
#define BLENDMODE_H_
//------------------------------------------------------------------------------
/**
    Blending related enumerations

    (C) 2011 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{
    /// type of texture blend mode
    /**
        LBT_COLOUR - The texture alpha does not have any effect <br>
        LBT_ALPHA - The texture alpha is taken into account <br>
    */
    enum LayerBlendType
    {
        LBT_COLOUR,
        LBT_ALPHA
    };

    /// list of valid texture blending operations, for use with Pass::SetTextureBlendOperation
    /**
        Cv - Color value result
        Ct - Texture color 
        Cf - Fragment color
        Cc - Constant color
        
        LBO_MODULATE    - Cv=Ct*Cf               LBT_COLOUR: Av=Af         LBT_ALPHA: Av=At*Af  <br>
        LBO_DECAL       -                        LBT_COLOUR: Cv=Ct,Av=Af   LBT_ALPHA: Cv=Cf*(1-At)+Ct*At Av=Af <br>
        LBO_ALPHA_BLEND - Cv=(Cf*(1-Ct))+(Cc*Ct) LBT_COLOUR: Av=Af         LBT_ALPHA: Av=At*Af <br>
        LBO_REPLACE     - Cv=Ct                  LBT_COLOUR: Av=Af         LBT_ALPHA: Av=At <br>
        LBO_ADD         - Cv=Cf+Ct               LBT_COLOUR: Av=Af         LBT_ALPHA: Av=At*Af <br>
    */
    enum LayerBlendOperation
    {
        LBO_REPLACE,
        LBO_ADD,
        LBO_MODULATE,
        LBO_ALPHA_BLEND,
        LBO_DECAL // only PSP
    };

    /** 
        Blending factors for manually blending objects with the scene. If there isn't a predefined
        SceneBlendType that you like, then you can specify the blending factors directly to affect the
        combination of object and the existing scene. See Material::setSceneBlending for more details.
    */
    enum SceneBlendFactor
    {
        SBF_ONE,
        SBF_ZERO,
        SBF_DEST_COLOUR,
        SBF_SOURCE_COLOUR,
        SBF_ONE_MINUS_DEST_COLOUR,
        SBF_ONE_MINUS_SOURCE_COLOUR,
        SBF_DEST_ALPHA,
        SBF_SOURCE_ALPHA,
        SBF_ONE_MINUS_DEST_ALPHA,
        SBF_ONE_MINUS_SOURCE_ALPHA,
        SBF_FIX // only PSP
    };
   
    /**
        Blending operations controls how objects are blended into the scene. The default operation
        is add (+) but by changing this you can change how drawn objects are blended into the
        existing scene.
    */
    enum SceneBlendOperation
    {
        SBO_ADD,
        SBO_SUBTRACT,
        SBO_REVERSE_SUBTRACT,
        SBO_MIN,
        SBO_MAX,
        SBO_ABS  // only PSP
    };
    
} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif