//------------------------------------------------------------------------------
//  materialparser.cpp
//  (C) 2015 Christian Bleicher
//------------------------------------------------------------------------------
#define STB_C_LEXER_IMPLEMENTATION

#include "materialparser.h"
#include "materialmanager.h"
#include "memoryallocatorconfig.h"
#include "debug.h"
#include "fswrapper.h"

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

static const unsigned int LexerTextBufferSize = 65536;

//------------------------------------------------------------------------------
/**
*/
MaterialParser::MaterialParser() :
    parserState(StateParseRoot),
    currentMaterial(NULL),
    currentPass(NULL),
    currentTextureUnitState(NULL)
{
    this->textBuffer = (char*)CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, LexerTextBufferSize);
}

//------------------------------------------------------------------------------
/**
*/
MaterialParser::~MaterialParser()
{
    CE_FREE(this->textBuffer);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialParser::ParseScript(const char* name)
{
    FileHandle fd = FSWrapper::Open(name, ReadAccess, Buffer, 0777);
    unsigned int fileSize = FSWrapper::GetFileSize(fd);
    void* fileBuffer = CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, fileSize);
    FSWrapper::Read(fd, fileBuffer, fileSize);
    FSWrapper::Close(fd);

    stb_c_lexer_init(&this->lexer, (char*)fileBuffer, (char*)((uintptr_t)fileBuffer + fileSize), this->textBuffer, LexerTextBufferSize);

    this->parserState = StateParseRoot;
    this->currentMaterial = NULL;
    this->currentPass = NULL;
    this->currentTextureUnitState = NULL;

    while (stb_c_lexer_get_token(&this->lexer))
    {
        if (CLEX_parse_error == this->lexer.token)
        {
            break;
        }

        switch (this->parserState)
        {
            case StateParseRoot:
                this->ParseRoot();
                break;
            case StateParseMaterial:
                this->ParseMaterial();
                break;
            case StateParsePass:
                this->ParsePass();
                break;
            case StateParseTextureUnit:
                this->ParseTextureUnitState();
                break;
        }
    }

    CE_FREE(fileBuffer);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialParser::ParseRoot()
{
    if (0 == strcmp(this->lexer.string, "material"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        char* matName = this->lexer.string;
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        if (this->lexer.token == '{')
        {
            this->parserState = StateParseMaterial;
            this->currentMaterial = MaterialManager::Instance()->CreateOrRetrieve(matName);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialParser::ParseMaterial()
{
    if (this->lexer.token == '}')
    {
        this->parserState = StateParseRoot;
    }
    else if (0 == strcmp(this->lexer.string, "pass"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        if (this->lexer.token == '{')
        {
            this->parserState = StateParsePass;
            this->currentPass = this->currentMaterial->CreatePass();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialParser::ParsePass()
{
    if (this->lexer.token == '}')
    {
        this->parserState = StateParseMaterial;
    }
    else if (0 == strcmp(this->lexer.string, "texture_unit"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        if (this->lexer.token == '{')
        {
            this->parserState = StateParseTextureUnit;
            this->currentTextureUnitState = this->currentPass->CreateTextureUnitState();
        }
    }
    else if (0 == strcmp(this->lexer.string, "lighting"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        if (0 == strcmp(this->lexer.string, "on"))
        {
            this->currentPass->SetLightingEnabled(true);
        }
        else if (0 == strcmp(this->lexer.string, "off"))
        {
            this->currentPass->SetLightingEnabled(false);
        }
    }
    else if (0 == strcmp(this->lexer.string, "fog_override"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        if (0 == strcmp(this->lexer.string, "true"))
        {
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float red = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float green = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float blue = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float start = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float end = (float)this->lexer.real_number;
            this->currentPass->SetFog(FOG_LINEAR, this->GetRGBAValue(red, green, blue, 1.0f), start, end);
        }
        else if (0 == strcmp(this->lexer.string, "false"))
        {
            this->currentPass->SetFog(FOG_NONE, 0, 0.0f, 0.0f);
        }
    }
    else if (0 == strcmp(this->lexer.string, "emissive"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        if (this->lexer.token == CLEX_id && 0 == strcmp(this->lexer.string, "vertexcolour"))
        {
            this->currentPass->SetVertexColourTracking(this->currentPass->GetVertexColourTracking() | TVC_EMISSIVE);
        }
        else
        {
            float red = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float green = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float blue = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float alpha = (float)this->lexer.real_number;
            this->currentPass->SetSelfIllumination(this->GetRGBAValue(red, green, blue, alpha));
        }
    }
    else if (0 == strcmp(this->lexer.string, "ambient"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        if (this->lexer.token == CLEX_id && 0 == strcmp(this->lexer.string, "vertexcolour"))
        {
            this->currentPass->SetVertexColourTracking(this->currentPass->GetVertexColourTracking() | TVC_AMBIENT);
        }
        else
        {
            float red = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float green = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float blue = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float alpha = (float)this->lexer.real_number;
            this->currentPass->SetAmbient(this->GetRGBAValue(red, green, blue, alpha));
        }
    }
    else if (0 == strcmp(this->lexer.string, "diffuse"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        if (this->lexer.token == CLEX_id && 0 == strcmp(this->lexer.string, "vertexcolour"))
        {
            this->currentPass->SetVertexColourTracking(this->currentPass->GetVertexColourTracking() | TVC_DIFFUSE);
        }
        else
        {
            float red = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float green = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float blue = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float alpha = (float)this->lexer.real_number;
            this->currentPass->SetDiffuse(this->GetRGBAValue(red, green, blue, alpha));
        }
    }
    else if (0 == strcmp(this->lexer.string, "specular"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        if (this->lexer.token == CLEX_id && 0 == strcmp(this->lexer.string, "vertexcolour"))
        {
            this->currentPass->SetVertexColourTracking(this->currentPass->GetVertexColourTracking() | TVC_SPECULAR);
        }
        else
        {
            float red = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float green = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float blue = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float alpha = (float)this->lexer.real_number;
            this->currentPass->SetSpecular(this->GetRGBAValue(red, green, blue, alpha));
        }
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        this->currentPass->SetShininess((float)this->lexer.real_number);
    }
    else if (0 == strcmp(this->lexer.string, "scene_blend"))
    {
        this->currentPass->SetSceneBlendingEnabled(true);
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        SceneBlendFactor source = this->GetSceneBlendFactorFromString(this->lexer.string);
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        SceneBlendFactor dest = this->GetSceneBlendFactorFromString(this->lexer.string);
        this->currentPass->SetSceneBlending(source, dest);
        if (SBF_FIX == source)
        {
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float red = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float green = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float blue = (float)this->lexer.real_number;
            this->currentPass->SetBlendingFixColors(this->GetRGBAValue(red, green, blue, 1.0f), this->currentPass->GetDestinationBlendingFixColor());
        }
        if (SBF_FIX == dest)
        {
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float red = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float green = (float)this->lexer.real_number;
            if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
            float blue = (float)this->lexer.real_number;
            this->currentPass->SetBlendingFixColors(this->currentPass->GetSourceBlendingFixColor(), this->GetRGBAValue(red, green, blue, 1.0f));
        }
    }
    else if (0 == strcmp(this->lexer.string, "cull_hardware"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        if (0 == strcmp(this->lexer.string, "none"))
        {
            this->currentPass->SetCullingMode(CULL_NONE);
        }
        else if (0 == strcmp(this->lexer.string, "clockwise"))
        {
            this->currentPass->SetCullingMode(CULL_CLOCKWISE);
        }
        else if (0 == strcmp(this->lexer.string, "anticlockwise"))
        {
            this->currentPass->SetCullingMode(CULL_ANTICLOCKWISE);
        }
    }
    else if (0 == strcmp(this->lexer.string, "depth_check"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        if (0 == strcmp(this->lexer.string, "on"))
        {
            this->currentPass->SetDepthCheckEnabled(true);
        }
        else if (0 == strcmp(this->lexer.string, "off"))
        {
            this->currentPass->SetDepthCheckEnabled(false);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialParser::ParseTextureUnitState()
{
    if (this->lexer.token == '}')
    {
        CE_ASSERT(this->currentTextureUnitState->GetTextureName().C_Str() != NULL, "MaterialParser::ParseTextureUnitState(): parse error, missing 'texture' in 'texture_unit'\n");
        this->parserState = StateParsePass;
    }
    else if (0 == strcmp(this->lexer.string, "texture"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        this->currentTextureUnitState->SetTextureName(this->lexer.string);
    }
    else if (0 == strcmp(this->lexer.string, "colour_op"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        LayerBlendOperation operation = LBO_MODULATE;
        if (0 == strcmp(this->lexer.string, "replace"))
        {
            operation = LBO_REPLACE;
        }
        else if (0 == strcmp(this->lexer.string, "add"))
        {
            operation = LBO_ADD;
        }
        else if (0 == strcmp(this->lexer.string, "alpha_blend"))
        {
            operation = LBO_ALPHA_BLEND;
        }
        else if (0 == strcmp(this->lexer.string, "decal"))
        {
            operation = LBO_DECAL;
        }
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        LayerBlendType type = LBT_COLOUR;
        if (0 == strcmp(this->lexer.string, "alpha"))
        {
            type = LBT_ALPHA;
        }
        this->currentTextureUnitState->SetTextureBlendOperation(type, operation);
    }
    else if (0 == strcmp(this->lexer.string, "filtering"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        FilterOptions minFilter = this->GetFilterOptionsFromString(this->lexer.string);
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        FilterOptions magFilter = this->GetFilterOptionsFromString(this->lexer.string);
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        FilterOptions mipFilter = this->GetFilterOptionsFromString(this->lexer.string);
        this->currentTextureUnitState->SetTextureFiltering(minFilter, magFilter, mipFilter);
    }
    else if (0 == strcmp(this->lexer.string, "scroll"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        float uMod = (float)this->lexer.real_number;
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        float vMod = (float)this->lexer.real_number;
        this->currentTextureUnitState->SetTextureScroll(uMod, vMod);
    }
    else if (0 == strcmp(this->lexer.string, "scale"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        float uScale = (float)this->lexer.real_number;
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        float vScale = (float)this->lexer.real_number;
        this->currentTextureUnitState->SetTextureScale(uScale, vScale);
    }
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
MaterialParser::GetRGBAValue(float red, float green, float blue, float alpha) const
{
    unsigned int colour = 0;
    colour |= (int)(alpha * 255.0f) << 24;
    colour |= (int)(blue * 255.0f) << 16;
    colour |= (int)(green * 255.0f) << 8;
    colour |= (int)(red * 255.0f);
    return colour;
}

//------------------------------------------------------------------------------
/**
*/
SceneBlendFactor
MaterialParser::GetSceneBlendFactorFromString(const char* blendFactor) const
{
    if (0 == strcmp(blendFactor, "one"))                    {return SBF_ONE;}
    if (0 == strcmp(blendFactor, "zero"))                   {return SBF_ZERO;}
    if (0 == strcmp(blendFactor, "dest_colour"))            {return SBF_DEST_COLOUR;}
    if (0 == strcmp(blendFactor, "src_colour"))             {return SBF_SOURCE_COLOUR;}
    if (0 == strcmp(blendFactor, "one_minus_dest_colour"))  {return SBF_ONE_MINUS_DEST_COLOUR;}
    if (0 == strcmp(blendFactor, "one_minus_src_colour"))   {return SBF_ONE_MINUS_SOURCE_COLOUR;}
    if (0 == strcmp(blendFactor, "dest_alpha"))             {return SBF_DEST_ALPHA;}
    if (0 == strcmp(blendFactor, "src_alpha"))              {return SBF_SOURCE_ALPHA;}
    if (0 == strcmp(blendFactor, "one_minus_dest_alpha"))   {return SBF_ONE_MINUS_DEST_ALPHA;}
    if (0 == strcmp(blendFactor, "one_minus_src_alpha"))    {return SBF_ONE_MINUS_SOURCE_ALPHA;}
    if (0 == strcmp(blendFactor, "fix"))                    {return SBF_FIX;}

    return SBF_ONE;
}

//------------------------------------------------------------------------------
/**
*/
FilterOptions
MaterialParser::GetFilterOptionsFromString(const char* filterOption) const
{
    if (0 == strcmp(filterOption, "none"))      {return FO_NONE;}
    if (0 == strcmp(filterOption, "point"))     {return FO_POINT;}
    if (0 == strcmp(filterOption, "linear"))    {return FO_LINEAR;}

    return FO_NONE;
}

} // namespace graphics
} // namespace chrissly
