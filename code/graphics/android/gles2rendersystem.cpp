//------------------------------------------------------------------------------
//  gles2rendersystem.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "gles2rendersystem.h"
#include "gles2mappings.h"
#include "light.h"
#include "textureunitstate.h"
#include "common.h"
#include "debug.h"

namespace chrissly
{

#define CE_GL_ENABLE_ERROR_CHECK (0)

GLES2RenderSystem* GLES2RenderSystem::Singleton = NULL;

//------------------------------------------------------------------------------
static const char* DefaultVertexShader =
    "attribute vec2 texCoordIn;\n"
    "attribute vec3 normal;\n"
    "attribute vec4 position;\n"
    "attribute vec4 positionMorphTarget;\n"
    "uniform mat4 worldMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "uniform mat4 projectionMatrix;\n"
    "uniform mat4 worldViewProjMatrix;\n"
    "uniform float morphWeight;\n"
    "uniform int fogMode;\n"
    "uniform float fogStart;\n"
    "uniform float fogEnd;\n"
    "uniform int lightingEnabled;"
    "varying float fogFactor;\n"
    "varying vec2 texCoordOut;\n"
    "varying vec3 worldNormal;\n"
    "varying vec3 worldPosition;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = worldViewProjMatrix * position;\n"
    "    texCoordOut = texCoordIn;\n"
    "    if (1 == fogMode)\n"
    "    {\n"
    "        // range based linear fog\n"
    "        fogFactor = clamp((fogEnd - length(viewMatrix * worldMatrix * position)) / (fogEnd - fogStart), 0.0, 1.0);\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "        fogFactor = 1.0;\n"
    "    }\n"
    "    if (1 == lightingEnabled)\n"
    "    {\n"
    "        worldNormal = normalize(worldMatrix * vec4(normal, 1.0)).xyz;\n"
    "        worldPosition = (worldMatrix * vec4(position.xyz, 1.0)).xyz;\n"
    "    }\n"
    "}\n";

static const char* DefaultFragmentShader =
    "precision mediump float;\n"
    "const int MaxLights = 4;\n"
    "varying float fogFactor;\n"
    "varying vec2 texCoordOut;\n"
    "varying vec3 worldNormal;\n"
    "varying vec3 worldPosition;\n"
    "uniform sampler2D texture;\n"
    "uniform float uMod;\n"
    "uniform float vMod;\n"
    "uniform float uScale;\n"
    "uniform float vScale;\n"
    "uniform vec3 fogColour;\n"
    "uniform vec3 cameraPosition;\n"
    "uniform highp int lightingEnabled;\n"
    "uniform mat4 lightParams[MaxLights];\n"
    "void main()\n"
    "{\n"
    "    vec4 colour = texture2D(texture, vec2(uScale * texCoordOut.x + uMod, vScale * texCoordOut.y + vMod));\n"
    "    if (1 == lightingEnabled)\n"
    "    {\n"
    "        vec3 L;\n"
    "        vec3 diffuse = vec3(0.0, 0.0, 0.0);\n"
    "        for (int i = 0; i < MaxLights; ++i)\n"
    "        {\n"
    "            L = normalize(vec3(lightParams[i][0][0], lightParams[i][0][1], lightParams[i][0][2]) - worldPosition);\n"
    "            diffuse += vec3(lightParams[i][1][0], lightParams[i][1][1], lightParams[i][1][2]) * max(0.0, dot(L, worldNormal));\n"
    "        }\n"
    "        colour.rgb = clamp(colour.rgb * diffuse, 0.0, 1.0);\n"
    "    }\n"
    "    gl_FragColor = mix(vec4(fogColour, 1.0), colour, fogFactor);\n"
    "}\n";

static const unsigned int MaxLights = 4;

//------------------------------------------------------------------------------
/**
*/
GLES2RenderSystem::GLES2RenderSystem() :
    ambientLight(0x00000000),
    worldMatrix(core::Matrix4::ZERO),
    viewMatrix(core::Matrix4::ZERO),
    projectionMatrix(core::Matrix4::ZERO),
    defaultGpuProgram(NULL),
    currentGpuProgram(NULL),
    numTextureUnits(0)
{
    Singleton = this;

    memset(this->glWorldMatrix, 0, 64);
    memset(this->glViewMatrix, 0, 64);
    memset(this->glProjectionMatrix, 0, 64);
    memset(this->glTextureMatrix, 0, 64);
    memset(this->glWorldViewProjectionMatrix, 0, 64);
}

//------------------------------------------------------------------------------
/**
*/
GLES2RenderSystem::~GLES2RenderSystem()
{
    Singleton = NULL;
}

//------------------------------------------------------------------------------
/**
*/
graphics::RenderWindow*
GLES2RenderSystem::_Initialise(void* customParams)
{
    graphics::RenderWindow* renderWindow = CE_NEW graphics::RenderWindow(customParams);
    renderWindow->Create();

    PrintGLString("Version", GL_VERSION);
    PrintGLString("Vendor", GL_VENDOR);
    PrintGLString("Renderer", GL_RENDERER);
    PrintGLString("Extensions", GL_EXTENSIONS);

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &this->numTextureUnits);
    CheckGlError("glGetIntegerv");
    CE_LOG("GL_MAX_TEXTURE_IMAGE_UNITS: %i\n", this->numTextureUnits);

    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    CheckGlError("glGetIntegerv");
    CE_LOG("GL_MAX_TEXTURE_SIZE: %i\n", maxTextureSize);

    GLint maxVaryingVectors;
    glGetIntegerv(GL_MAX_VARYING_VECTORS, &maxVaryingVectors);
    CheckGlError("glGetIntegerv");
    CE_LOG("GL_MAX_VARYING_VECTORS: %i\n", maxVaryingVectors);

    glEnable(GL_SCISSOR_TEST);
    CheckGlError("glEnable");

    this->defaultGpuProgram = CE_NEW GLES2GpuProgram(DefaultVertexShader, DefaultFragmentShader);
    this->currentGpuProgram = this->defaultGpuProgram;

    return renderWindow;
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::Shutdown()
{
    CE_DELETE this->defaultGpuProgram;

    CE_LOG("GLES2RenderSystem::Shutdown\n");
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::_SetRenderTarget(graphics::RenderTarget *target)
{

}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::_SetViewport(graphics::Viewport *vp)
{
    int width = vp->GetActualWidth();
    int height = vp->GetActualHeight();
    int left = vp->GetActualLeft();
    int top = vp->GetActualTop();

    glViewport(left, top, width, height);
    CheckGlError("glViewport");
    glDepthRangef(0.0f, 1.0f);
    CheckGlError("glDepthRangef");
    glScissor(left, top, width, height);
    CheckGlError("glScissor");

    if (vp->GetClearEveryFrame())
    {
        float red, green, blue, alpha;
        GLES2Mappings::Get(vp->GetBackgroundColour(), red, green, blue, alpha);
        glClearColor(red, green, blue, alpha);
        CheckGlError("glClearColor");
        glClear(GLES2Mappings::Get((graphics::FrameBufferType)vp->GetClearBuffers()));
        CheckGlError("glClear");
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::_SetWorldMatrix(const core::Matrix4& m)
{
    this->worldMatrix = m;
    GLES2Mappings::MakeGLMatrix(this->glWorldMatrix, m);
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::_SetViewMatrix(const core::Matrix4& m)
{
    this->viewMatrix = m;
    GLES2Mappings::MakeGLMatrix(this->glViewMatrix, m);
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::_SetProjectionMatrix(const core::Matrix4& m)
{
    this->projectionMatrix = m;
    GLES2Mappings::MakeGLMatrix(this->glProjectionMatrix, m);
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::_SetTextureMatrix(const core::Matrix4& xform)
{
    GLES2Mappings::MakeGLMatrix(this->glTextureMatrix, xform);
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::SetMatrices()
{
    glUniformMatrix4fv(this->currentGpuProgram->GetUniformLocation(graphics::GpuProgramParameters::ACT_WORLD_MATRIX), 1, GL_FALSE, this->glWorldMatrix);
    CheckGlError("glUniformMatrix4fv");
    glUniformMatrix4fv(this->currentGpuProgram->GetUniformLocation(graphics::GpuProgramParameters::ACT_VIEW_MATRIX), 1, GL_FALSE, this->glViewMatrix);
    CheckGlError("glUniformMatrix4fv");
    glUniformMatrix4fv(this->currentGpuProgram->GetUniformLocation(graphics::GpuProgramParameters::ACT_PROJECTION_MATRIX), 1, GL_FALSE, this->glProjectionMatrix);
    CheckGlError("glUniformMatrix4fv");
    GLES2Mappings::MakeGLMatrix(this->glWorldViewProjectionMatrix, this->projectionMatrix * this->viewMatrix * this->worldMatrix);
    glUniformMatrix4fv(this->currentGpuProgram->GetUniformLocation(graphics::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX), 1, GL_FALSE, this->glWorldViewProjectionMatrix);
    CheckGlError("glUniformMatrix4fv");
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::_Render(graphics::SubEntity* renderable)
{
    this->SetMatrices();

    if (graphics::VAT_MORPH == renderable->GetSubMesh()->GetVertexAnimationType())
    {
        glUniform1f(this->currentGpuProgram->GetUniformLocation(graphics::GpuProgramParameters::ACT_MORPH_WEIGHT), renderable->GetMorphWeight());
        CheckGlError("glUniform1f");

        graphics::VertexData* vertexData = renderable->_GetHardwareVertexAnimVertexData();

        GLint vertexTexCoordHandle = this->currentGpuProgram->GetAttributeLocation(graphics::VES_TEXTURE_COORDINATES);
        glVertexAttribPointer(vertexTexCoordHandle, 2, GL_FLOAT, GL_FALSE, 72, vertexData->vertexBuffer);
        CheckGlError("glVertexAttribPointer");
        glEnableVertexAttribArray(vertexTexCoordHandle);
        CheckGlError("glEnableVertexAttribArray");

        GLint vertexNormalHandle = this->currentGpuProgram->GetAttributeLocation(graphics::VES_NORMAL);
        glVertexAttribPointer(vertexNormalHandle, 3, GL_FLOAT, GL_FALSE, 72, (unsigned char*)vertexData->vertexBuffer + 12);
        CheckGlError("glVertexAttribPointer");
        glEnableVertexAttribArray(vertexNormalHandle);
        CheckGlError("glEnableVertexAttribArray");

        GLint vertexPositionHandle = this->currentGpuProgram->GetAttributeLocation(graphics::VES_POSITION);
        glVertexAttribPointer(vertexPositionHandle, 3, GL_FLOAT, GL_FALSE, 72, (unsigned char*)vertexData->vertexBuffer + 24);
        CheckGlError("glVertexAttribPointer");
        glEnableVertexAttribArray(vertexPositionHandle);
        CheckGlError("glEnableVertexAttribArray");

        GLint vertexPositionMorphTargetHandle = this->currentGpuProgram->GetAttributeLocation(graphics::VES_POSITION_MORPH_TARGET);
        glVertexAttribPointer(vertexPositionMorphTargetHandle, 3, GL_FLOAT, GL_FALSE, 72, (unsigned char*)vertexData->vertexBuffer + 60);
        CheckGlError("glVertexAttribPointer");
        glEnableVertexAttribArray(vertexPositionMorphTargetHandle);
        CheckGlError("glEnableVertexAttribArray");

        glDrawArrays(GL_TRIANGLES, 0, vertexData->vertexCount);
        CheckGlError("glDrawArrays");
    }
    else
    {
        graphics::VertexData* vertexData = renderable->GetSubMesh()->vertexData;

        GLint vertexTexCoordHandle = this->currentGpuProgram->GetAttributeLocation(graphics::VES_TEXTURE_COORDINATES);
        glVertexAttribPointer(vertexTexCoordHandle, 2, GL_FLOAT, GL_FALSE, 36, vertexData->vertexBuffer);
        CheckGlError("glVertexAttribPointer: texturecoords");
        glEnableVertexAttribArray(vertexTexCoordHandle);
        CheckGlError("glEnableVertexAttribArray: texturecoords");

        GLint vertexNormalHandle = this->currentGpuProgram->GetAttributeLocation(graphics::VES_NORMAL);
        glVertexAttribPointer(vertexNormalHandle, 3, GL_FLOAT, GL_FALSE, 36, (unsigned char*)vertexData->vertexBuffer + 12);
        CheckGlError("glVertexAttribPointer: normal");
        glEnableVertexAttribArray(vertexNormalHandle);
        CheckGlError("glEnableVertexAttribArray: normal");

        GLint vertexPositionHandle = this->currentGpuProgram->GetAttributeLocation(graphics::VES_POSITION);
        glVertexAttribPointer(vertexPositionHandle, 3, GL_FLOAT, GL_FALSE, 36, (unsigned char*)vertexData->vertexBuffer + 24);
        CheckGlError("glVertexAttribPointer: position");
        glEnableVertexAttribArray(vertexPositionHandle);
        CheckGlError("glEnableVertexAttribArray: position");

        glDrawArrays(GL_TRIANGLES, 0, vertexData->vertexCount);
        CheckGlError("glDrawArrays");
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::_BeginFrame()
{

}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::_EndFrame()
{

}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::_SetPass(graphics::Pass* pass)
{
    // set gpu program to use
    if (pass->IsProgrammable())
    {
        this->currentGpuProgram = pass->GetGpuProgram();
    }
    else
    {
        this->currentGpuProgram = this->defaultGpuProgram;
    }
    glUseProgram(this->currentGpuProgram->GetProgramHandle());
    CheckGlError("glUseProgram");

    // scene blending parameters
    if (pass->GetSceneBlendingEnabled())
    {
        glEnable(GL_BLEND);
        glBlendEquation(GLES2Mappings::Get(pass->GetSceneBlendingOperation()));
        glBlendFunc(GLES2Mappings::Get(pass->GetSourceBlendFactor()), GLES2Mappings::Get(pass->GetDestBlendFactor()));
    }
    else
    {
        glDisable(GL_BLEND);
    }

    // depth check
    pass->GetDepthCheckEnabled() ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

    // depth write
    glDepthMask(pass->GetDepthWriteEnabled() ? GL_TRUE : GL_FALSE);

    // culling mode
    switch (pass->GetCullingMode())
    {
        case graphics::CULL_NONE:
            glDisable(GL_CULL_FACE);
            break;
        case graphics::CULL_CLOCKWISE:
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CW);
            glCullFace(GL_FRONT);
            break;
        case graphics::CULL_ANTICLOCKWISE:
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_FRONT);
            break;
    }

    // texture unit parameters
    unsigned short textureUnitState;
    for (textureUnitState = 0; textureUnitState < pass->GetNumTextureUnitStates() && textureUnitState < this->numTextureUnits; ++textureUnitState)
    {
        graphics::TextureUnitState* tus = pass->GetTextureUnitState(textureUnitState);
        graphics::Texture* tex = tus->GetTexture();
        glActiveTexture(GL_TEXTURE0 + textureUnitState);
        glBindTexture(GL_TEXTURE_2D, tex->GetName());

        GLint min, mag;
        GLES2Mappings::Get(tus->GetTextureFiltering(graphics::FT_MIN), tus->GetTextureFiltering(graphics::FT_MAG), tus->GetTextureFiltering(graphics::FT_MIP), min, mag);
        if (0 == tex->GetNumMipmaps())
        {
            if (min == GL_NEAREST_MIPMAP_NEAREST || min == GL_NEAREST_MIPMAP_LINEAR)
            {
                min = GL_NEAREST;
            }
            else if (min == GL_LINEAR_MIPMAP_NEAREST || min == GL_LINEAR_MIPMAP_LINEAR)
            {
                min = GL_LINEAR;
            }
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLES2Mappings::Get(tus->GetTextureAddressingMode().u));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLES2Mappings::Get(tus->GetTextureAddressingMode().v));
    }

    // set default shader parameters
    if (!pass->IsProgrammable())
    {
        graphics::GpuProgramParameters* params = this->currentGpuProgram->GetDefaultParameters();

        if (pass->GetNumTextureUnitStates() > 0)
        {
            graphics::TextureUnitState* tus = pass->GetTextureUnitState(0);
            params->SetNamedConstant("uMod", tus->GetTextureUScroll());
            params->SetNamedConstant("vMod", tus->GetTextureVScroll());
            params->SetNamedConstant("uScale", tus->GetTextureUScale());
            params->SetNamedConstant("vScale", tus->GetTextureVScale());
        }

        params->SetNamedConstant("fogMode", (int)pass->GetFogMode());
        if (graphics::FOG_LINEAR == pass->GetFogMode())
        {
            params->SetNamedConstant("fogStart", pass->GetFogStart());
            params->SetNamedConstant("fogEnd", pass->GetFogEnd());
            core::Vector3 fogColour;
            float alpha;
            GLES2Mappings::Get(pass->GetFogColour(), fogColour.x, fogColour.y, fogColour.z, alpha);
            params->SetNamedConstant("fogColour", fogColour);
        }

        params->SetNamedConstant("lightingEnabled", (int)pass->GetLightingEnabled());
        if (pass->GetLightingEnabled())
        {
            core::Matrix4 invViewMat = this->viewMatrix.Inverse();
            core::Vector3 cameraPosition(invViewMat[0][3], invViewMat[1][3], invViewMat[2][3]);
            params->SetNamedConstant("cameraPosition", cameraPosition);
            params->SetNamedConstant("lightParams[0]", this->defaultLightShaderParams, 4);
        }
    }

    // apply shader parameters
    graphics::GpuNamedConstants* constantDefs = this->currentGpuProgram->GetConstantDefinitions();
    unsigned int i;
    for (i = 0; i < constantDefs->map.capacity; ++i)
    {
        LinkedList* it = ((core::Chain*)DynamicArrayGet(&constantDefs->map.entries, i))->list;
        while (it != NULL)
        {
            graphics::GpuConstantDefinition* def = (graphics::GpuConstantDefinition*)((core::KeyValuePair*)it->data)->value;
            switch (def->constType)
            {
                case graphics::GCT_INT1:
                    glUniform1iv(def->location, def->arraySize, (int*)def->buffer);
                    break;
                case graphics::GCT_FLOAT1:
                    glUniform1fv(def->location, def->arraySize, (float*)def->buffer);
                    break;
                case graphics::GCT_FLOAT3:
                    glUniform3fv(def->location, def->arraySize, (float*)def->buffer);
                    break;
                case graphics::GCT_FLOAT4:
                    glUniform4fv(def->location, def->arraySize, (float*)def->buffer);
                    break;
                case graphics::GCT_MATRIX_4X4:
                    glUniformMatrix4fv(def->location, def->arraySize, GL_FALSE, (float*)def->buffer);
                    break;
                case graphics::GCT_SAMPLER2D:
                    glUniform1iv(def->location, def->arraySize, (int*)def->buffer);
                    break;
            }

            it = it->next;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::_UseLights(core::HashTable* lights)
{
    unsigned int lightIndex = 0;
    unsigned int i;
    for (i = 0; i < lights->capacity && lightIndex < MaxLights; ++i)
    {
        LinkedList* it = ((core::Chain*)DynamicArrayGet(&lights->entries, i))->list;
        while (it != NULL && lightIndex < MaxLights)
        {
            graphics::Light* light = (graphics::Light*)((core::KeyValuePair*)it->data)->value;
            const core::Vector3 position = light->GetPosition();
            this->defaultLightShaderParams[lightIndex][0][0] = position.x;
            this->defaultLightShaderParams[lightIndex][0][1] = position.y;
            this->defaultLightShaderParams[lightIndex][0][2] = position.z;
            float red, green, blue, alpha;
            GLES2Mappings::Get(light->GetDiffuseColour(), red, green, blue, alpha);
            this->defaultLightShaderParams[lightIndex][1][0] = red;
            this->defaultLightShaderParams[lightIndex][1][1] = green;
            this->defaultLightShaderParams[lightIndex][1][2] = blue;
            GLES2Mappings::Get(light->GetSpecularColour(), red, green, blue, alpha);
            this->defaultLightShaderParams[lightIndex][2][0] = red;
            this->defaultLightShaderParams[lightIndex][2][1] = green;
            this->defaultLightShaderParams[lightIndex][2][2] = blue;
            ++lightIndex;
            it = it->next;
        }
    }

    for (i = lightIndex; i < MaxLights; ++i)
    {
        this->defaultLightShaderParams[lightIndex] = core::Matrix4::ZERO;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::SetAmbientLight(unsigned int colour)
{
    this->ambientLight = colour;
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::_NotifyMorphKeyFrameBuild()
{

}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::PrintGLString(const char* name, GLenum s)
{
    const char* v = (const char*)glGetString(s);
    CE_LOG("GL %s = %s\n", name, v);
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::CheckGlError(const char* op)
{
#if CE_GL_ENABLE_ERROR_CHECK
    GLint error;
    for (error = glGetError(); error; error = glGetError())
    {
        CE_LOG("after %s() glError (0x%x)\n", op, error);
    }
#endif
}

} // namespace chrissly
