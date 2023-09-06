//------------------------------------------------------------------------------
//  gles2rendersystem.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "gles2rendersystem.h"
#include "gles2mappings.h"
#include "gles2defaultshaders.h"
#include "gles2debug.h"
#include "light.h"
#include "textureunitstate.h"
#include "miscutils.h"
#include "common.h"
#include "debug.h"

namespace chrissly
{

GLES2RenderSystem* GLES2RenderSystem::Singleton = NULL;

static const unsigned int MaxLights = 4U;

//------------------------------------------------------------------------------
/**
*/
GLES2RenderSystem::GLES2RenderSystem() :
    ambientLight(0x00000000),
    worldMatrix(core::Matrix4::ZERO),
    viewMatrix(core::Matrix4::ZERO),
    projectionMatrix(core::Matrix4::ZERO),
    defaultGpuProgram(NULL),
    defaultGpuProgramFog(NULL),
    defaultGpuProgramLit(NULL),
    defaultGpuProgramLitFog(NULL),
    defaultGpuProgramMorphAnim(NULL),
    defaultGpuProgramShadowCaster(NULL),
    defaultGpuProgramShadowReceiver(NULL),
    currentGpuProgram(NULL),
    numTextureUnits(0),
    maxVertexAttribs(0)
{
    Singleton = this;

    memset(this->glWorldMatrix, 0, sizeof(this->glWorldMatrix));
    memset(this->glViewMatrix, 0, sizeof(this->glViewMatrix));
    memset(this->glProjectionMatrix, 0, sizeof(this->glProjectionMatrix));
    memset(this->glTextureMatrix, 0, sizeof(this->glTextureMatrix));
    memset(this->glWorldViewProjectionMatrix, 0, sizeof(this->glWorldViewProjectionMatrix));
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
graphics::RenderWindow* const
GLES2RenderSystem::Initialise(void* const customParams)
{
    graphics::RenderWindow* renderWindow = CE_NEW graphics::RenderWindow(customParams);
    renderWindow->Create();

    CE_PRINT_GL_STRING("Version", GL_VERSION);
    CE_PRINT_GL_STRING("Vendor", GL_VENDOR);
    CE_PRINT_GL_STRING("Renderer", GL_RENDERER);
    CE_PRINT_GL_STRING("Extensions", GL_EXTENSIONS);

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &this->numTextureUnits);
    CE_GL_ERROR_CHECK("glGetIntegerv");
    CE_LOG("GL_MAX_TEXTURE_IMAGE_UNITS: %i\n", this->numTextureUnits);

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &this->maxVertexAttribs);
    CE_GL_ERROR_CHECK("glGetIntegerv");
    CE_LOG("GL_MAX_VERTEX_ATTRIBS: %i\n", this->maxVertexAttribs);

    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    CE_GL_ERROR_CHECK("glGetIntegerv");
    CE_LOG("GL_MAX_TEXTURE_SIZE: %i\n", maxTextureSize);

    GLint maxVaryingVectors;
    glGetIntegerv(GL_MAX_VARYING_VECTORS, &maxVaryingVectors);
    CE_GL_ERROR_CHECK("glGetIntegerv");
    CE_LOG("GL_MAX_VARYING_VECTORS: %i\n", maxVaryingVectors);

    glEnable(GL_SCISSOR_TEST);
    CE_GL_ERROR_CHECK("glEnable");
    glDepthFunc(GL_LEQUAL);
    CE_GL_ERROR_CHECK("glDepthFunc");

    this->defaultGpuProgram = CE_NEW GLES2GpuProgram(DefaultVertexShader, DefaultFragmentShader);
    this->defaultGpuProgramFog = CE_NEW GLES2GpuProgram(DefaultVertexShaderFog, DefaultFragmentShaderFog);
    this->defaultGpuProgramLit = CE_NEW GLES2GpuProgram(DefaultVertexShaderLit, DefaultFragmentShaderLit);
    this->defaultGpuProgramLitFog = CE_NEW GLES2GpuProgram(DefaultVertexShaderLitFog, DefaultFragmentShaderLitFog);
    this->defaultGpuProgramMorphAnim = CE_NEW GLES2GpuProgram(DefaultVertexShaderMorphAnim, DefaultFragmentShaderMorphAnim);
    this->defaultGpuProgramShadowCaster = CE_NEW GLES2GpuProgram(DefaultVertexShaderShadowCaster, DefaultFragmentShaderShadowCaster);
    this->defaultGpuProgramShadowReceiver = CE_NEW GLES2GpuProgram(DefaultVertexShaderShadowReceiver, DefaultFragmentShaderShadowReceiver);
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
    this->defaultGpuProgram = NULL;
    CE_DELETE this->defaultGpuProgramFog;
    this->defaultGpuProgramFog = NULL;
    CE_DELETE this->defaultGpuProgramLit;
    this->defaultGpuProgramLit = NULL;
    CE_DELETE this->defaultGpuProgramLitFog;
    this->defaultGpuProgramLitFog = NULL;
    CE_DELETE this->defaultGpuProgramMorphAnim;
    this->defaultGpuProgramMorphAnim = NULL;
    CE_DELETE this->defaultGpuProgramShadowCaster;
    this->defaultGpuProgramShadowCaster = NULL;
    CE_DELETE this->defaultGpuProgramShadowReceiver;
    this->defaultGpuProgramShadowReceiver = NULL;
    this->currentGpuProgram = NULL;

    CE_LOG("GLES2RenderSystem::Shutdown\n");
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::SetRenderTarget(graphics::RenderTarget* const target)
{
    if ('EGLW' == target->GetType())
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0U);
    }
    else if ('GLRT' == target->GetType())
    {
        GLES2RenderTexture* renderTexture = (GLES2RenderTexture*)target;
        glBindFramebuffer(GL_FRAMEBUFFER, renderTexture->GetFBO());
    }
    CE_GL_ERROR_CHECK("glBindFramebuffer");
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::SetViewport(graphics::Viewport* const vp)
{
    int width = vp->GetActualWidth();
    int height = vp->GetActualHeight();
    int left = vp->GetActualLeft();
    int top = vp->GetActualTop();

    glViewport(left, top, width, height);
    CE_GL_ERROR_CHECK("glViewport");
    glDepthRangef(0.0f, 1.0f);
    CE_GL_ERROR_CHECK("glDepthRangef");
    glScissor(left, top, width, height);
    CE_GL_ERROR_CHECK("glScissor");

    if (vp->GetClearEveryFrame())
    {
        float red, green, blue, alpha;
        ce_colour_convert_u32_to_float(vp->GetBackgroundColour(), red, green, blue, alpha);
        glClearColor(red, green, blue, alpha);
        CE_GL_ERROR_CHECK("glClearColor");
        glClear(GLES2Mappings::Get((graphics::FrameBufferType)vp->GetClearBuffers()));
        CE_GL_ERROR_CHECK("glClear");
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::SetWorldMatrix(const core::Matrix4& m)
{
    this->worldMatrix = m;
    GLES2Mappings::MakeGLMatrix(this->glWorldMatrix, m);
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::SetViewMatrix(const core::Matrix4& m)
{
    this->viewMatrix = m;
    GLES2Mappings::MakeGLMatrix(this->glViewMatrix, m);
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::SetProjectionMatrix(const core::Matrix4& m)
{
    this->projectionMatrix = m;
    GLES2Mappings::MakeGLMatrix(this->glProjectionMatrix, m);
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::SetTextureMatrix(const core::Matrix4& xform)
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
    CE_GL_ERROR_CHECK("glUniformMatrix4fv");
    glUniformMatrix4fv(this->currentGpuProgram->GetUniformLocation(graphics::GpuProgramParameters::ACT_VIEW_MATRIX), 1, GL_FALSE, this->glViewMatrix);
    CE_GL_ERROR_CHECK("glUniformMatrix4fv");
    glUniformMatrix4fv(this->currentGpuProgram->GetUniformLocation(graphics::GpuProgramParameters::ACT_PROJECTION_MATRIX), 1, GL_FALSE, this->glProjectionMatrix);
    CE_GL_ERROR_CHECK("glUniformMatrix4fv");
    GLES2Mappings::MakeGLMatrix(this->glWorldViewProjectionMatrix, this->projectionMatrix * this->viewMatrix * this->worldMatrix);
    glUniformMatrix4fv(this->currentGpuProgram->GetUniformLocation(graphics::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX), 1, GL_FALSE, this->glWorldViewProjectionMatrix);
    CE_GL_ERROR_CHECK("glUniformMatrix4fv");
    glUniformMatrix4fv(this->currentGpuProgram->GetUniformLocation(graphics::GpuProgramParameters::ACT_TEXTURE_MATRIX), 1, GL_FALSE, this->glTextureMatrix);
    CE_GL_ERROR_CHECK("glUniformMatrix4fv");
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::Render(graphics::SubEntity* const renderable)
{
    this->SetMatrices();

    if (graphics::VAT_MORPH == renderable->GetSubMesh()->GetVertexAnimationType())
    {
        glUniform1f(this->currentGpuProgram->GetUniformLocation(graphics::GpuProgramParameters::ACT_MORPH_WEIGHT), renderable->GetMorphWeight());
        CE_GL_ERROR_CHECK("glUniform1f");

        graphics::HardwareVertexBuffer* vertexBuffer = renderable->GetMorphVertexData()->vertexBuffer;
        GLsizei stride = (GLsizei)vertexBuffer->GetBytesPerVertex();
        unsigned char* buffer = (unsigned char*)vertexBuffer->Map();

        GLuint vertexTexCoordHandle = this->currentGpuProgram->GetAttributeLocation(graphics::VES_TEXTURE_COORDINATES);
        if (vertexTexCoordHandle < this->maxVertexAttribs)
        {
            glVertexAttribPointer(vertexTexCoordHandle, 2, GL_FLOAT, GL_FALSE, stride, buffer);
            CE_GL_ERROR_CHECK("glVertexAttribPointer");
            glEnableVertexAttribArray(vertexTexCoordHandle);
            CE_GL_ERROR_CHECK("glEnableVertexAttribArray");
        }

        GLuint vertexNormalHandle = this->currentGpuProgram->GetAttributeLocation(graphics::VES_NORMAL);
        if (vertexNormalHandle < this->maxVertexAttribs)
        {
            glVertexAttribPointer(vertexNormalHandle, 3, GL_FLOAT, GL_FALSE, stride, buffer + 12U);
            CE_GL_ERROR_CHECK("glVertexAttribPointer");
            glEnableVertexAttribArray(vertexNormalHandle);
            CE_GL_ERROR_CHECK("glEnableVertexAttribArray");
        }

        GLuint vertexPositionHandle = this->currentGpuProgram->GetAttributeLocation(graphics::VES_POSITION);
        glVertexAttribPointer(vertexPositionHandle, 3, GL_FLOAT, GL_FALSE, stride, buffer + 24U);
        CE_GL_ERROR_CHECK("glVertexAttribPointer");
        glEnableVertexAttribArray(vertexPositionHandle);
        CE_GL_ERROR_CHECK("glEnableVertexAttribArray");

        GLuint vertexPositionMorphTargetHandle = this->currentGpuProgram->GetAttributeLocation(graphics::VES_POSITION_MORPH_TARGET);
        glVertexAttribPointer(vertexPositionMorphTargetHandle, 3, GL_FLOAT, GL_FALSE, stride, buffer + 60U);
        CE_GL_ERROR_CHECK("glVertexAttribPointer");
        glEnableVertexAttribArray(vertexPositionMorphTargetHandle);
        CE_GL_ERROR_CHECK("glEnableVertexAttribArray");

        glDrawArrays(GL_TRIANGLES, 0, vertexBuffer->GetNumVertices());
        CE_GL_ERROR_CHECK("glDrawArrays");
    }
    else
    {
        graphics::HardwareVertexBuffer* vertexBuffer = renderable->GetSubMesh()->vertexData->vertexBuffer;
        GLsizei stride = (GLsizei)vertexBuffer->GetBytesPerVertex();

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->GetName());
        CE_GL_ERROR_CHECK("glBindBuffer");

        GLuint vertexTexCoordHandle = this->currentGpuProgram->GetAttributeLocation(graphics::VES_TEXTURE_COORDINATES);
        if (vertexTexCoordHandle < this->maxVertexAttribs)
        {
            glVertexAttribPointer(vertexTexCoordHandle, 2, GL_FLOAT, GL_FALSE, stride, (void*)0U);
            CE_GL_ERROR_CHECK("glVertexAttribPointer: texturecoords");
            glEnableVertexAttribArray(vertexTexCoordHandle);
            CE_GL_ERROR_CHECK("glEnableVertexAttribArray: texturecoords");
        }

        GLuint vertexNormalHandle = this->currentGpuProgram->GetAttributeLocation(graphics::VES_NORMAL);
        if (vertexNormalHandle < this->maxVertexAttribs)
        {
            glVertexAttribPointer(vertexNormalHandle, 3, GL_FLOAT, GL_FALSE, stride, (void*)12U);
            CE_GL_ERROR_CHECK("glVertexAttribPointer: normal");
            glEnableVertexAttribArray(vertexNormalHandle);
            CE_GL_ERROR_CHECK("glEnableVertexAttribArray: normal");
        }

        GLuint vertexPositionHandle = this->currentGpuProgram->GetAttributeLocation(graphics::VES_POSITION);
        glVertexAttribPointer(vertexPositionHandle, 3, GL_FLOAT, GL_FALSE, stride, (void*)24U);
        CE_GL_ERROR_CHECK("glVertexAttribPointer: position");
        glEnableVertexAttribArray(vertexPositionHandle);
        CE_GL_ERROR_CHECK("glEnableVertexAttribArray: position");

        glDrawArrays(GL_TRIANGLES, 0, vertexBuffer->GetNumVertices());
        CE_GL_ERROR_CHECK("glDrawArrays");

        glBindBuffer(GL_ARRAY_BUFFER, 0U);
        CE_GL_ERROR_CHECK("glBindBuffer");
    }

    GLuint vai;
    for (vai = 0U; vai < this->maxVertexAttribs; ++vai)
    {
        glDisableVertexAttribArray(vai);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::BeginFrame()
{

}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::EndFrame()
{

}

//------------------------------------------------------------------------------
/**
*/
void
GLES2RenderSystem::SetPass(graphics::Pass* const pass)
{
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
    glDepthMask(pass->GetDepthWriteEnabled());

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
        GLenum texType = GLES2Mappings::Get(tex->GetType());
        glActiveTexture(GL_TEXTURE0 + textureUnitState);
        glBindTexture(texType, tex->GetName());

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
        glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, min);
        glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, mag);

        glTexParameteri(texType, GL_TEXTURE_WRAP_S, GLES2Mappings::Get(tus->GetTextureAddressingMode().u));
        glTexParameteri(texType, GL_TEXTURE_WRAP_T, GLES2Mappings::Get(tus->GetTextureAddressingMode().v));
    }

    // set gpu program to use
    if (pass->IsProgrammable())
    {
        this->currentGpuProgram = pass->GetGpuProgram();
    }
    else
    {
        bool lit = pass->GetLightingEnabled();
        bool fog = (graphics::FOG_LINEAR == pass->GetFogMode());
        if (pass->IsSkeletalAnimationIncluded())
        {
            // Todo: set skeletal animation shader
        }
        else if (pass->IsMorphAnimationIncluded())
        {
            this->currentGpuProgram = this->defaultGpuProgramMorphAnim;
        }
        else if (!fog && !lit)
        {
            this->currentGpuProgram = this->defaultGpuProgram;
        }
        else if (fog && !lit)
        {
            this->currentGpuProgram = this->defaultGpuProgramFog;
        }
        else if (!fog && lit)
        {
            this->currentGpuProgram = this->defaultGpuProgramLit;
        }
        else if (fog && lit)
        {
            this->currentGpuProgram = this->defaultGpuProgramLitFog;
        }

        graphics::GpuProgramParameters* params = this->currentGpuProgram->GetDefaultParameters();

        if (pass->GetNumTextureUnitStates() > 0U)
        {
            graphics::TextureUnitState* tus = pass->GetTextureUnitState(0U);
            params->SetNamedConstant("uMod", tus->GetTextureUScroll());
            params->SetNamedConstant("vMod", tus->GetTextureVScroll());
            params->SetNamedConstant("uScale", tus->GetTextureUScale());
            params->SetNamedConstant("vScale", tus->GetTextureVScale());
        }

        if (fog)
        {
            core::Vector3 fogColour;
            float alpha;
            ce_colour_convert_u32_to_float(pass->GetFogColour(), fogColour.x, fogColour.y, fogColour.z, alpha);
            params->SetNamedConstant("fogColour", fogColour);
            params->SetNamedConstant("fogMode", (int)pass->GetFogMode());
            params->SetNamedConstant("fogStart", pass->GetFogStart());
            params->SetNamedConstant("fogEnd", pass->GetFogEnd());
        }

        if (lit)
        {
            core::Matrix4 invViewMat = this->viewMatrix.Inverse();
            core::Vector3 cameraPosition(invViewMat[0U][3U], invViewMat[1U][3U], invViewMat[2U][3U]);
            params->SetNamedConstant("cameraPosition", cameraPosition);
            params->SetNamedConstant("lightParams[0]", this->defaultLightShaderParams, MaxLights);
        }
    }

    glUseProgram(this->currentGpuProgram->GetProgramHandle());
    CE_GL_ERROR_CHECK("glUseProgram");

    // apply shader parameters
    graphics::GpuNamedConstants* constantDefs = this->currentGpuProgram->GetConstantDefinitions();
    unsigned int i;
    for (i = 0U; i < constantDefs->map.bucket_count; ++i)
    {
        ce_linked_list* it = constantDefs->map.buckets[i];
        while (it != NULL)
        {
            graphics::GpuConstantDefinition* def = (graphics::GpuConstantDefinition*)((ce_key_value_pair*)it->data)->value;
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
                case graphics::GCT_SAMPLERCUBE:
                    glUniform1iv(def->location, def->arraySize, (int*)def->buffer);
                    break;
                default:
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
GLES2RenderSystem::ProcessLights(ce_hash_table* const lights)
{
    unsigned int lightIndex = 0U;
    unsigned int i;
    for (i = 0U; i < lights->bucket_count && lightIndex < MaxLights; ++i)
    {
        ce_linked_list* it = lights->buckets[i];
        while (it != NULL && lightIndex < MaxLights)
        {
            graphics::Light* light = (graphics::Light*)((ce_key_value_pair*)it->data)->value;

            const core::Vector3& position = light->GetPosition();
            this->defaultLightShaderParams[lightIndex][0U][0U] = position.x;
            this->defaultLightShaderParams[lightIndex][0U][1U] = position.y;
            this->defaultLightShaderParams[lightIndex][0U][2U] = position.z;

            float red, green, blue, alpha;
            ce_colour_convert_u32_to_float(light->GetDiffuseColour(), red, green, blue, alpha);
            this->defaultLightShaderParams[lightIndex][1U][0U] = red;
            this->defaultLightShaderParams[lightIndex][1U][1U] = green;
            this->defaultLightShaderParams[lightIndex][1U][2U] = blue;

            ce_colour_convert_u32_to_float(light->GetSpecularColour(), red, green, blue, alpha);
            this->defaultLightShaderParams[lightIndex][2U][0U] = red;
            this->defaultLightShaderParams[lightIndex][2U][1U] = green;
            this->defaultLightShaderParams[lightIndex][2U][2U] = blue;

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
const core::Matrix4* const
GLES2RenderSystem::GetDefaultLightShaderParams() const
{
    return this->defaultLightShaderParams;
}

//------------------------------------------------------------------------------
/**
*/
GLES2GpuProgram *const
GLES2RenderSystem::GetDefaultShadowCasterGpuProgram() const
{
    return this->defaultGpuProgramShadowCaster;
}

//------------------------------------------------------------------------------
/**
*/
GLES2GpuProgram *const
GLES2RenderSystem::GetDefaultShadowReceiverGpuProgram() const
{
    return this->defaultGpuProgramShadowReceiver;
}

} // namespace chrissly
