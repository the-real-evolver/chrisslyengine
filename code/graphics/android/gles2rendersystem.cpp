//------------------------------------------------------------------------------
//  gles2rendersystem.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "gles2rendersystem.h"
#include "gles2mappings.h"
#include "textureunitstate.h"
#include "common.h"
#include "debug.h"

namespace chrissly
{

GLES2RenderSystem* GLES2RenderSystem::Singleton = NULL;

//------------------------------------------------------------------------------
const char* GLES2RenderSystem::DefaultVertexShader =
    "attribute vec2 texCoordIn;\n"
    "attribute vec3 normal;\n"
    "attribute vec4 position;\n"
    "attribute vec4 positionMorphTarget;\n"
    "uniform mat4 worldMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "uniform mat4 projectionMatrix;\n"
    "uniform mat4 worldViewProjMatrix;\n"
    "uniform float morphWeight;\n"
    "varying vec2 texCoordOut;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = worldViewProjMatrix * position;\n"
    "    texCoordOut = texCoordIn;\n"
    "}\n";

const char* GLES2RenderSystem::DefaultFragmentShader =
    "precision mediump float;\n"
    "varying vec2 texCoordOut;\n"
    "uniform sampler2D texture;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = texture2D(texture, texCoordOut);\n"
    "}\n";

//------------------------------------------------------------------------------
/**
*/
GLES2RenderSystem::GLES2RenderSystem() :
    ambientLight(0x00000000),
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
        CheckGlError("glVertexAttribPointer");
        glEnableVertexAttribArray(vertexTexCoordHandle);
        CheckGlError("glEnableVertexAttribArray");

        GLint vertexNormalHandle = this->currentGpuProgram->GetAttributeLocation(graphics::VES_NORMAL);
        glVertexAttribPointer(vertexNormalHandle, 3, GL_FLOAT, GL_FALSE, 36, (unsigned char*)vertexData->vertexBuffer + 12);
        CheckGlError("glVertexAttribPointer");
        glEnableVertexAttribArray(vertexNormalHandle);
        CheckGlError("glEnableVertexAttribArray");

        GLint vertexPositionHandle = this->currentGpuProgram->GetAttributeLocation(graphics::VES_POSITION);
        glVertexAttribPointer(vertexPositionHandle, 3, GL_FLOAT, GL_FALSE, 36, (unsigned char*)vertexData->vertexBuffer + 24);
        CheckGlError("glVertexAttribPointer");
        glEnableVertexAttribArray(vertexPositionHandle);
        CheckGlError("glEnableVertexAttribArray");

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
    if (pass->GetNumTextureUnitStates() > 0)
    {
        graphics::TextureUnitState* tus = pass->GetTextureUnitState(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tus->GetTexture()->GetName());

        GLint min, mag;
        GLES2Mappings::Get(tus->GetTextureFiltering(graphics::FT_MIN), tus->GetTextureFiltering(graphics::FT_MAG), tus->GetTextureFiltering(graphics::FT_MIP), min, mag);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLES2Mappings::Get(tus->GetTextureAddressingMode().u));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLES2Mappings::Get(tus->GetTextureAddressingMode().v));

        glUniform1i(this->currentGpuProgram->GetTextureUniformLocation(), 0 /* index of the textureunit */);
    }

    // apply shader parameters
    graphics::GpuNamedConstants* constantDefs = this->currentGpuProgram->GetConstantDefinitions();
    unsigned int i;
    for (i = 0; i < constantDefs->map.capacity; i++)
    {
        LinkedList* it = ((core::Chain*)DynamicArrayGet(&constantDefs->map.entries, i))->list;
        while (it != NULL)
        {
            graphics::GpuConstantDefinition* def = (graphics::GpuConstantDefinition*)((core::KeyValuePair*)it->data)->value;
            switch (def->constType)
            {
                case graphics::GCT_MATRIX_4X4:
                    glUniformMatrix4fv(def->location, 1, GL_FALSE, (float*)def->buffer);
                    break;
                case graphics::GCT_FLOAT1:
                    glUniform1f(def->location, *(float*)def->buffer);
                    break;
                case graphics::GCT_SAMPLER2D:
                    glUniform1i(def->location, *(int*)def->buffer);
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
    GLint error;
    for (error = glGetError(); error; error = glGetError())
    {
        CE_LOG("after %s() glError (0x%x)\n", op, error);
    }
}

} // namespace chrissly
