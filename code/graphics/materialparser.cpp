//------------------------------------------------------------------------------
//  materialparser.cpp
//  (C) 2015 Christian Bleicher
//------------------------------------------------------------------------------
#define STB_C_LEXER_IMPLEMENTATION

#include "materialparser.h"
#include "materialmanager.h"
#include "gpuprogrammanager.h"
#include "memoryallocatorconfig.h"
#include "debug.h"
#include "fswrapper.h"

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

static const unsigned int LexerTextBufferSize = 65536U;

//------------------------------------------------------------------------------
/**
*/
MaterialParser::MaterialParser() :
    parserState(STATE_PARSE_ROOT),
    currentMaterial(NULL),
    currentPass(NULL),
    currentTextureUnitState(NULL)
{
    memset(&this->lexer, 0, sizeof(this->lexer));
    this->textBuffer = (char*)CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, LexerTextBufferSize);
    CE_ASSERT(this->textBuffer != NULL, "MaterialParser::MaterialParser(): failed to allocate '%i' bytes\n", LexerTextBufferSize);
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
MaterialParser::ParseScript(const char* const name)
{
    FileHandle fd = FSWrapper::Open(name, READ_ACCESS, BUFFER, 0777);
    unsigned int fileSize = FSWrapper::GetFileSize(fd);
    void* fileBuffer = CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, fileSize);
    CE_ASSERT(fileBuffer != NULL, "MaterialParser::ParseScript(): failed to allocate '%i' bytes\n", fileSize);
    FSWrapper::Read(fd, fileBuffer, fileSize);
    FSWrapper::Close(fd);

    stb_c_lexer_init(&this->lexer, (char*)fileBuffer, (char*)((uintptr_t)fileBuffer + fileSize), this->textBuffer, LexerTextBufferSize);

    this->parserState = STATE_PARSE_ROOT;
    this->currentMaterial = NULL;
    this->currentPass = NULL;
    this->currentTextureUnitState = NULL;

    while (stb_c_lexer_get_token(&this->lexer) != 0)
    {
        if (CLEX_parse_error == this->lexer.token)
        {
            break;
        }

        switch (this->parserState)
        {
            case STATE_PARSE_ROOT:
                this->ParseRoot();
                break;
            case STATE_PARSE_MATERIAL:
                this->ParseMaterial();
                break;
            case STATE_PARSE_PASS:
                this->ParsePass();
                break;
            case STATE_PARSE_TEXTUREUNIT:
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
        if ('{' == this->lexer.token)
        {
            this->parserState = STATE_PARSE_MATERIAL;
            this->currentMaterial = MaterialManager::Instance()->Create(matName);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialParser::ParseMaterial()
{
    if ('}' == this->lexer.token)
    {
        this->parserState = STATE_PARSE_ROOT;
    }
    else if (0 == strcmp(this->lexer.string, "pass"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        if ('{' == this->lexer.token)
        {
            this->parserState = STATE_PARSE_PASS;
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
    if ('}' == this->lexer.token)
    {
        this->parserState = STATE_PARSE_MATERIAL;
    }
    else if (0 == strcmp(this->lexer.string, "texture_unit"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        if ('{' == this->lexer.token)
        {
            this->parserState = STATE_PARSE_TEXTUREUNIT;
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
    else if (0 == strcmp(this->lexer.string, "alpha_test"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        ComparisonFunction func = this->GetComparisonFunctionFromString(this->lexer.string);
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        float ref = (float)this->lexer.real_number;
        this->currentPass->SetAlphaFunction(func, ref);
    }
    else if (0 == strcmp(this->lexer.string, "alpha_to_coverage"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        if (0 == strcmp(this->lexer.string, "on"))
        {
            this->currentPass->SetAlphaToCoverageEnabled(true);
        }
        else if (0 == strcmp(this->lexer.string, "off"))
        {
            this->currentPass->SetAlphaToCoverageEnabled(false);
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
    else if (0 == strcmp(this->lexer.string, "depth_write"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        if (0 == strcmp(this->lexer.string, "on"))
        {
            this->currentPass->SetDepthWriteEnabled(true);
        }
        else if (0 == strcmp(this->lexer.string, "off"))
        {
            this->currentPass->SetDepthWriteEnabled(false);
        }
    }
    else if (0 == strcmp(this->lexer.string, "gpu_program"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        this->currentPass->SetGpuProgram(GpuProgramManager::Instance()->GetByName(this->lexer.string));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialParser::ParseTextureUnitState()
{
    if ('}' == this->lexer.token)
    {
        CE_ASSERT(this->currentTextureUnitState->GetTextureName().Size() > 0U, "MaterialParser::ParseTextureUnitState(): parse error, missing 'texture' in 'texture_unit'\n");
        this->parserState = STATE_PARSE_PASS;
    }
    else if (0 == strcmp(this->lexer.string, "texture"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        this->currentTextureUnitState->SetTextureName(this->lexer.string);
    }
    else if (0 == strcmp(this->lexer.string, "cubic_texture"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) { return; }
        this->currentTextureUnitState->SetTextureName(this->lexer.string, TEX_TYPE_CUBE_MAP);
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
    else if (0 == strcmp(this->lexer.string, "env_map"))
    {
        if (0 == stb_c_lexer_get_token(&this->lexer)) {return;}
        if (0 == strcmp(this->lexer.string, "spherical"))
        {
            this->currentTextureUnitState->SetTextureMappingMode(TextureUnitState::TMM_ENVIRONMENT_MAP);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
MaterialParser::GetRGBAValue(float red, float green, float blue, float alpha) const
{
    unsigned int colour = 0U;
    colour |= (unsigned int)(alpha * 255.0f) << 24U;
    colour |= (unsigned int)(blue * 255.0f) << 16U;
    colour |= (unsigned int)(green * 255.0f) << 8U;
    colour |= (unsigned int)(red * 255.0f);
    return colour;
}

//------------------------------------------------------------------------------
/**
*/
SceneBlendFactor
MaterialParser::GetSceneBlendFactorFromString(const char* const blendFactor) const
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
MaterialParser::GetFilterOptionsFromString(const char* const filterOption) const
{
    if (0 == strcmp(filterOption, "none"))      {return FO_NONE;}
    if (0 == strcmp(filterOption, "point"))     {return FO_POINT;}
    if (0 == strcmp(filterOption, "linear"))    {return FO_LINEAR;}

    return FO_NONE;
}

//------------------------------------------------------------------------------
/**
*/
ComparisonFunction
MaterialParser::GetComparisonFunctionFromString(const char* const compFunc) const
{
    if (0 == strcmp(compFunc, "never"))         {return CF_NEVER;}
    if (0 == strcmp(compFunc, "less"))          {return CF_LESS;}
    if (0 == strcmp(compFunc, "equal"))         {return CF_EQUAL;}
    if (0 == strcmp(compFunc, "less_equal"))    {return CF_LESS_EQUAL;}
    if (0 == strcmp(compFunc, "greater"))       {return CF_GREATER;}
    if (0 == strcmp(compFunc, "not_equal"))     {return CF_NOT_EQUAL;}
    if (0 == strcmp(compFunc, "greater_equal")) {return CF_GREATER_EQUAL;}
    if (0 == strcmp(compFunc, "always"))        {return CF_ALWAYS;}

    return CF_ALWAYS;
}

} // namespace graphics
} // namespace chrissly
