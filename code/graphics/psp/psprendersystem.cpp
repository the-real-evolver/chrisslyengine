//------------------------------------------------------------------------------
//  psprendersystem.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "psprendersystem.h"
#include "psp/pspmappings.h"
#include "common.h"
#include <stdio.h>
#include <pspgum.h>

namespace chrissly
{

PSPRenderSystem* PSPRenderSystem::Singleton = NULL;

/// the GE store the commands for processing in this array
unsigned int __attribute__((aligned(16))) PSPRenderSystem::DisplayList[262144];

//------------------------------------------------------------------------------
/**
*/
PSPRenderSystem::PSPRenderSystem() : ambientLight(0x00000000)
{
    Singleton = this;
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
graphics::RenderWindow*
PSPRenderSystem::_Initialise(void* customParams)
{
    sceGuInit();

    graphics::RenderWindow* renderWindow = CE_NEW graphics::RenderWindow();
    renderWindow->Create();
    
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
PSPRenderSystem::_SetRenderTarget(graphics::RenderTarget *target)
{
    sceGuDrawBufferList(PSPMappings::Get(target->GetFormat()),target->GetBuffer(), target->GetWidth());
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::_SetViewport(graphics::Viewport *vp)
{
    sceGuClearDepth(0);
    sceGuClearColor(vp->GetBackgroundColour());
    sceGuOffset(2048 - vp->GetActualLeft(), 2048 - vp->GetActualTop());
    sceGuViewport(2048, 2048, vp->GetActualWidth(), vp->GetActualHeight());
    sceGuClear(PSPMappings::Get((graphics::FrameBufferType)vp->GetClearBuffers()));
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::_SetWorldMatrix(const core::Matrix4& m)
{
    this->worldMat = PSPMappings::MakePSPMatrix(m);
    sceGuSetMatrix(GU_MODEL, &this->worldMat);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::_SetViewMatrix(const core::Matrix4& m)
{
    this->viewMat = PSPMappings::MakePSPMatrix(m);
    sceGuSetMatrix(GU_VIEW, &this->viewMat);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::_SetProjectionMatrix(const core::Matrix4& m)
{
    this->projMat = PSPMappings::MakePSPMatrix(m);
    sceGuSetMatrix(GU_PROJECTION, &this->projMat);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::_SetTextureMatrix(const core::Matrix4& xform)
{
    this->texMat = PSPMappings::MakePSPMatrix(xform);
    sceGuSetMatrix(GU_TEXTURE, &this->texMat);
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::_Render(graphics::SubEntity* renderable)
{
    sceGumDrawArray(GU_TRIANGLES,
                    GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D,
                    renderable->GetSubMesh()->_vertexCount, 0, 
                    renderable->GetSubMesh()->_vertexBuffer); 
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::_SetPass(graphics::Pass* pass)
{
    // scene blending parameters
    if (pass->GetSceneBlendingEnabled())
    {
        sceGuEnable(GU_BLEND);
        sceGuBlendFunc(PSPMappings::Get(pass->GetSceneBlendingOperation()),
                        PSPMappings::Get(pass->GetSourceBlendFactor()), PSPMappings::Get(pass->GetDestBlendFactor()),
                        pass->GetSourceBlendingFixColor(), pass->GetDestinationBlendingFixColor());
    }
    else
    {
        sceGuDisable(GU_BLEND);
    }
    
    // depth check
    pass->GetDepthCheckEnabled() ? sceGuEnable(GU_DEPTH_TEST) : sceGuDisable(GU_DEPTH_TEST);
    
    // culling mode
    switch (pass->GetCullingMode())
    {
        case graphics::CULL_NONE:
            sceGuDisable(GU_CULL_FACE);
            break;
        case graphics::CULL_CLOCKWISE:
            sceGuEnable(GU_CULL_FACE);
            sceGuFrontFace(GU_CW);
            break;
        case graphics::CULL_ANTICLOCKWISE:
            sceGuEnable(GU_CULL_FACE);
            sceGuFrontFace(GU_CCW);
            break;
    }
    
    // lighting
    if (pass->GetLightingEnabled())
    {
        sceGuEnable(GU_LIGHTING);
        sceGuAmbient(this->ambientLight);
        graphics::TrackVertexColourType tvct = (graphics::TrackVertexColourType)pass->GetVertexColourTracking();
        sceGuColorMaterial(PSPMappings::Get(tvct));
        if (tvct & graphics::TVC_AMBIENT)
        {
            sceGuSendCommandi(0x55, pass->GetAmbient() & 0xffffff);
            sceGuSendCommandi(0x58, pass->GetAmbient() >> 24);
        }
        if (tvct & graphics::TVC_DIFFUSE)
        {
            sceGuSendCommandi(0x56, pass->GetDiffuse() & 0xffffff);
        }
        if (tvct & graphics::TVC_SPECULAR)
        {
            sceGuSendCommandi(0x57, pass->GetSpecular() & 0xffffff);
            sceGuSpecular(pass->GetShininess());
        }
        if (tvct & graphics::TVC_EMISSIVE)
        {
            sceGuSendCommandi(0x54, pass->GetSelfIllumination() & 0xffffff);
        }
    }
    else
    {
        sceGuDisable(GU_LIGHTING);
    }
   
    // texture unit parameters    
    if (pass->GetNumTextureUnitStates() > 0)
    {
        graphics::TextureUnitState* tus = pass->GetTextureUnitState(0);
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
        sceGuTexMapMode(PSPMappings::Get(tus->GetTextureMappingMode()), 0, 0);
        sceGuTexProjMapMode(PSPMappings::Get(tus->GetTextureProjectionMappingMode()));

        sceGuTexMode(PSPMappings::Get(texture->GetFormat()), 0, 0, texture->GetSwizzleEnabled());
        sceGuTexImage(0, texture->GetWidth(), texture->GetHeight(), texture->GetWidth(), texture->GetBuffer());
    }
    else
    {
        sceGuDisable(GU_TEXTURE_2D);
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
PSPRenderSystem::SetAmbientLight(unsigned int colour)
{
    this->ambientLight = colour;
}
    
//------------------------------------------------------------------------------
/**
*/
void* 
PSPRenderSystem::GetDisplayList() const
{
    return (void*)DisplayList;
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderSystem::FillInterleaved(void const* src0, void const* src1, void* dst, unsigned short stride, unsigned int dstLen)
{
    char* pDst = (char*)dst;
    char const* pSrc0 = (char const*)src0;
    char const* pSrc1 = (char const*)src1;
    unsigned int doubleStride = stride << 1;

    unsigned int i;
    while (dstLen)
    {
        for (i = 0; i < stride; i++)
        {
            *pDst = *pSrc0++;
            *(pDst + stride) = *pSrc1++;
            pDst++;
        }
        pDst += stride;
        dstLen -= doubleStride;
    }
}

} // namespace chrissly