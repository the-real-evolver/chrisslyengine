//------------------------------------------------------------------------------
//  psprendersystem.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "psprendersystem.h"
#include "pspmappings.h"
#include "common.h"
#include "animationtrack.h"
#include "vertexdata.h"
#include <stdio.h>
#include <pspgum.h>
#include <psputils.h>

namespace chrissly
{

PSPRenderSystem* PSPRenderSystem::Singleton = NULL;

/// the GE store the commands for processing in this array
static unsigned int __attribute__((aligned(16U))) DisplayList[262144U];

static const int MaxLights = 4;

//------------------------------------------------------------------------------
/**
*/
PSPRenderSystem::PSPRenderSystem() :
    ambientLight(0x00000000)
{
    Singleton = this;
    memset(&this->lightPos, 0, sizeof(this->lightPos));
    memset(&this->lightDir, 0, sizeof(this->lightDir));
    memset(&this->viewMat, 0, sizeof(this->viewMat));
    memset(&this->projMat, 0, sizeof(this->projMat));
    memset(&this->worldMat, 0, sizeof(this->worldMat));
    memset(&this->texMat, 0, sizeof(this->texMat));
}

//------------------------------------------------------------------------------
/**
*/
PSPRenderSystem::~PSPRenderSystem()
{
    Singleton = NULL;
}

//------------------------------------------------------------------------------
/**
*/
graphics::RenderWindow* const
PSPRenderSystem::Initialise(void* const customParams)
{
    sceGuInit();

    graphics::RenderWindow* renderWindow = CE_NEW graphics::RenderWindow();
    renderWindow->Create();

    sceGuStart(GU_DIRECT, DisplayList);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuEnable(GU_CLIP_PLANES);
    sceGuDepthRange(65535, 0);
    sceGuDepthFunc(GU_GEQUAL);
    sceGuShadeModel(GU_SMOOTH);
    sceGuFinish();
    sceGuSync(0, 0);
    sceGuDisplay(GU_TRUE);

    return renderWindow;
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::Shutdown()
{
    sceGuTerm();
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::SetRenderTarget(graphics::RenderTarget* const target)
{
    sceGuDrawBufferList(PSPMappings::Get(target->GetFormat()), target->GetBuffer(), target->GetWidth());
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::SetViewport(graphics::Viewport* const vp)
{
    int width = vp->GetActualWidth();
    int height = vp->GetActualHeight();
    int left = vp->GetActualLeft();
    int top = vp->GetActualTop();

    sceGuOffset(2048U - ((unsigned int)width >> 1U) - left, 2048U - ((unsigned int)height >> 1U) - top);
    sceGuViewport(2048, 2048, width, height);
    sceGuScissor(left, top, left + width, top + height);

    if (vp->GetClearEveryFrame())
    {
        sceGuClearDepth(0U);
        sceGuClearColor(vp->GetBackgroundColour());
        sceGuClear(PSPMappings::Get((graphics::FrameBufferType)vp->GetClearBuffers()));
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::SetWorldMatrix(const core::Matrix4& m)
{
    this->worldMat = PSPMappings::MakePSPMatrix(m);
    sceGuSetMatrix(GU_MODEL, &this->worldMat);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::SetViewMatrix(const core::Matrix4& m)
{
    this->viewMat = PSPMappings::MakePSPMatrix(m);
    sceGuSetMatrix(GU_VIEW, &this->viewMat);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::SetProjectionMatrix(const core::Matrix4& m)
{
    this->projMat = PSPMappings::MakePSPMatrix(m);
    sceGuSetMatrix(GU_PROJECTION, &this->projMat);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::SetTextureMatrix(const core::Matrix4& xform)
{
    this->texMat = PSPMappings::MakePSPMatrix(xform);
    sceGuSetMatrix(GU_TEXTURE, &this->texMat);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::Render(graphics::SubEntity* const renderable)
{
    if (graphics::VAT_MORPH == renderable->GetSubMesh()->GetVertexAnimationType())
    {
        float morphWeight = renderable->GetMorphWeight();
        sceGuMorphWeight(0, 1.0f - morphWeight);
        sceGuMorphWeight(1, morphWeight);

        graphics::HardwareVertexBuffer* vertexBuffer = renderable->GetMorphVertexData()->vertexBuffer;
        sceGumDrawArray(GU_TRIANGLES,
                        GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D | GU_VERTICES(2) | GU_WEIGHTS(2),
                        vertexBuffer->GetNumVertices(), 0,
                        vertexBuffer->Map());
    }
    else
    {
        graphics::HardwareVertexBuffer* vertexBuffer = renderable->GetSubMesh()->vertexData->vertexBuffer;
        sceGumDrawArray(GU_TRIANGLES,
                        GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D,
                        vertexBuffer->GetNumVertices(), 0,
                        vertexBuffer->Map());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::BeginFrame()
{
    sceGuStart(GU_DIRECT, DisplayList);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::EndFrame()
{
    sceGuFinish();
    sceGuSync(0, 0);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::SetPass(graphics::Pass* const pass)
{
    // scene blending parameters
    if (pass->GetSceneBlendingEnabled())
    {
        sceGuEnable(GU_BLEND);
        graphics::SceneBlendFactor sourceBlendFactor = pass->GetSourceBlendFactor();
        graphics::SceneBlendFactor destBlendFactor = pass->GetDestBlendFactor();
        unsigned int sourceFixColor = (graphics::SBF_ONE == sourceBlendFactor) ? 0xffffffff : pass->GetSourceBlendingFixColor();
        unsigned int destFixColor = (graphics::SBF_ONE == destBlendFactor) ? 0xffffffff : pass->GetDestinationBlendingFixColor();
        sceGuBlendFunc(PSPMappings::Get(pass->GetSceneBlendingOperation()),
                        PSPMappings::Get(sourceBlendFactor), PSPMappings::Get(destBlendFactor),
                        sourceFixColor, destFixColor);
    }
    else
    {
        sceGuDisable(GU_BLEND);
    }

    // depth check
    pass->GetDepthCheckEnabled() ? sceGuEnable(GU_DEPTH_TEST) : sceGuDisable(GU_DEPTH_TEST);

    // depth write
    sceGuDepthMask(pass->GetDepthWriteEnabled() ? GU_FALSE : GU_TRUE);

    // culling mode
    switch (pass->GetCullingMode())
    {
        case graphics::CULL_NONE:
            sceGuDisable(GU_CULL_FACE);
            break;
        case graphics::CULL_CLOCKWISE:
            sceGuEnable(GU_CULL_FACE);
            sceGuFrontFace(GU_CCW);
            break;
        case graphics::CULL_ANTICLOCKWISE:
            sceGuEnable(GU_CULL_FACE);
            sceGuFrontFace(GU_CW);
            break;
    }

    // texture unit parameters
    if (pass->GetNumTextureUnitStates() > 0U)
    {
        graphics::TextureUnitState* tus = pass->GetTextureUnitState(0U);
        graphics::Texture* texture = tus->GetTexture();

        sceGuEnable(GU_TEXTURE_2D);
        sceGuTexScale(tus->GetTextureUScale(), tus->GetTextureVScale());
        sceGuTexOffset(tus->GetTextureUScroll(), tus->GetTextureVScroll());

        int min, mag;
        PSPMappings::Get(tus->GetTextureFiltering(graphics::FT_MIN), tus->GetTextureFiltering(graphics::FT_MAG), tus->GetTextureFiltering(graphics::FT_MIP), min, mag);
        sceGuTexFilter(min, mag);

        graphics::LayerBlendOperation lbo; graphics::LayerBlendType lbt;
        tus->GetTextureBlendOperation(lbt, lbo);
        sceGuTexFunc(PSPMappings::Get(lbo), PSPMappings::Get(lbt));

        sceGuTexWrap(PSPMappings::Get(tus->GetTextureAddressingMode().u), PSPMappings::Get(tus->GetTextureAddressingMode().v));
        sceGuTexMapMode(PSPMappings::Get(tus->GetTextureMappingMode()), 0U, 0U);
        sceGuTexProjMapMode(PSPMappings::Get(tus->GetTextureProjectionMappingMode()));

        int numMipmaps = texture->GetNumMipmaps();
        sceGuTexMode(PSPMappings::Get(texture->GetFormat()), numMipmaps, 0, texture->GetSwizzleEnabled());
        sceGuTexImage(0, texture->GetWidth(), texture->GetHeight(), texture->GetWidth(), texture->GetBuffer());
        int i;
        for (i = 1; i <= numMipmaps; ++i)
        {
            PSPTexture::MipmapInfo* info = texture->GetMipmapInfo(i);
            sceGuTexImage(i, info->width, info->height, info->width, info->buffer);
        }
    }
    else
    {
        sceGuDisable(GU_TEXTURE_2D);
    }

    // lighting
    if (pass->GetLightingEnabled())
    {
        sceGuEnable(GU_LIGHTING);
        sceGuAmbient(this->ambientLight);
        sceGuSendCommandi(0x54, pass->GetSelfIllumination() & 0xffffff);
        sceGuSendCommandi(0x55, pass->GetAmbient() & 0xffffff);
        sceGuSendCommandi(0x58, pass->GetAmbient() >> 24U);
        sceGuSendCommandi(0x56, pass->GetDiffuse() & 0xffffff);
        sceGuSendCommandi(0x57, pass->GetSpecular() & 0xffffff);
        sceGuSpecular(pass->GetShininess());
        sceGuColorMaterial(PSPMappings::Get((graphics::TrackVertexColourType)pass->GetVertexColourTracking()));
    }
    else
    {
        sceGuDisable(GU_LIGHTING);
    }

    // fog parameters
    if (graphics::FOG_LINEAR == pass->GetFogMode())
    {
        sceGuEnable(GU_FOG);
        sceGuFog(pass->GetFogStart(), pass->GetFogEnd(), pass->GetFogColour());
    }
    else
    {
        sceGuDisable(GU_FOG);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::ProcessLights(ce_hash_table* const lights)
{
    int lightIndex = 0;

    unsigned int i;
    for (i = 0U; i < lights->bucket_count && lightIndex < MaxLights; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(lights, i);
        while (it != NULL && lightIndex < MaxLights)
        {
            graphics::Light* light = (graphics::Light*)((ce_key_value_pair*)it->data)->value;

            graphics::Light::LightTypes type = light->GetType();

            if (graphics::Light::LT_DIRECTIONAL != type)
            {
                const core::Vector3& position = light->GetPosition();
                this->lightPos.x = position.x;
                this->lightPos.y = position.y;
                this->lightPos.z = position.z;
            }

            if (graphics::Light::LT_POINT != type)
            {
                const core::Vector3& direction = light->GetDirection();
                this->lightDir.x = -direction.x;
                this->lightDir.y = -direction.y;
                this->lightDir.z = -direction.z;
            }

            if (graphics::Light::LT_SPOTLIGHT == type)
            {
                sceGuLightSpot(lightIndex, &this->lightDir, light->GetSpotlightFalloff(), light->GetSpotlightOuterAngle());
            }

            int components = 0;
            unsigned int diffuse = light->GetDiffuseColour();
            if (diffuse > 0x00000000)
            {
                components |= GU_DIFFUSE;
                sceGuLightColor(lightIndex, GU_DIFFUSE, diffuse);
            }
            unsigned int specular = light->GetSpecularColour();
            if (specular > 0x00000000)
            {
                components |= GU_SPECULAR;
                sceGuLightColor(lightIndex, GU_SPECULAR, specular);
            }

            sceGuLight(lightIndex, PSPMappings::Get(type), components, graphics::Light::LT_DIRECTIONAL == type ? &this->lightDir : &this->lightPos);
            sceGuLightAtt(lightIndex, light->GetAttenuationConstant(), light->GetAttenuationLinear(), light->GetAttenuationQuadric());
            sceGuEnable(GU_LIGHT0 + lightIndex);
            ++lightIndex;

            it = it->next;
        }
    }

    for (i = lightIndex; i < (unsigned int)MaxLights; ++i)
    {
        sceGuDisable(GU_LIGHT0 + i);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::SetAmbientLight(unsigned int colour)
{
    this->ambientLight = colour;
}

//------------------------------------------------------------------------------
/**
*/
void* const
PSPRenderSystem::GetDisplayList() const
{
    return (void*)DisplayList;
}

} // namespace chrissly