#ifndef MATERIALPARSER_H_
#define MATERIALPARSER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::MaterialParser

    (C) 2015 Christian Bleicher
*/
#include "stb_c_lexer.h"
#include "material.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class MaterialParser
{
public:
    /// default constructor
    MaterialParser();
    /// destructor
    ~MaterialParser();
    /// parse material script and create materials
    void ParseScript(const char* const name);

private:
    /// defines the state of the parser
    enum ParserState
    {
        STATE_PARSE_ROOT,
        STATE_PARSE_MATERIAL,
        STATE_PARSE_PASS,
        STATE_PARSE_TEXTUREUNIT
    };

    /// parse root of the material description
    void ParseRoot();
    /// parse material from material description
    void ParseMaterial();
    /// parse pass from the material description
    void ParsePass();
    /// parse texture unit state from the material description
    void ParseTextureUnitState();
    /// gets the uint rgb colour value
    unsigned int GetRGBAValue(float red, float green, float blue, float alpha) const;
    /// gets the blend factor enum value from the parsed string
    SceneBlendFactor GetSceneBlendFactorFromString(const char* const blendFactor) const;
    /// gets the filter option enum value from the parsed string
    FilterOptions GetFilterOptionsFromString(const char* const filterOption) const;

    ParserState parserState;
    stb_lexer lexer;
    char* textBuffer;
    Material* currentMaterial;
    Pass* currentPass;
    TextureUnitState* currentTextureUnitState;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
