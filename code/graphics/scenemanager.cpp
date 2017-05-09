//------------------------------------------------------------------------------
//  scenemanager.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "graphicssystem.h"
#include "scenemanager.h"
#include "meshmanager.h"
#include "debug.h"
#include <stdio.h>

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

SceneManager* SceneManager::Singleton = NULL;

//------------------------------------------------------------------------------
/**
*/
SceneManager::SceneManager() :
    sceneRoot(NULL),
    ambientLight(0x00000000),
    suppressRenderStateChanges(false),
    illuminationStage(IRS_NONE),
    shadowTechnique(SHADOWTYPE_NONE),
    shadowTextureConfigDirty(true),
    shadowTexture(NULL),
    shadowRenderTexture(NULL),
    shadowCamera(NULL),
    shadowRttPass(NULL),
    shadowPass(NULL)
{
    Singleton = this;

    ce_hash_table_init(&this->cameras, 2);
    ce_hash_table_init(&this->lights, 4);
    ce_dynamic_array_init(&this->entities, 256);
    ce_dynamic_array_init(&this->sceneNodes, 256);

    this->renderQueueOpaque.Initialise(128);
    this->renderQueueTransparent.Initialise(64);
    this->renderQueueShadowReceiver.Initialise(64);

    this->destRenderSystem = GraphicsSystem::Instance()->GetRenderSystem();
}

//------------------------------------------------------------------------------
/**
*/
SceneManager::~SceneManager()
{
    Singleton = NULL;

    this->ClearScene();

    CE_DELETE this->sceneRoot;

    this->DestroyAllCameras();

    if (!this->shadowTextureConfigDirty)
    {
        CE_DELETE this->shadowTexture;
        CE_DELETE this->shadowRenderTexture;
        CE_DELETE this->shadowCamera;
        CE_DELETE this->shadowRttPass;
        CE_DELETE this->shadowPass;
    }
}

//------------------------------------------------------------------------------
/**
*/
Camera*
SceneManager::CreateCamera(const char* name)
{
    Camera* camera = CE_NEW Camera();

    ce_hash_table_insert(&this->cameras, name, strlen(name), camera);

    return camera;
}

//------------------------------------------------------------------------------
/**
*/
Camera*
SceneManager::GetCamera(const char* name) const
{
    return (Camera*)ce_hash_table_find(&this->cameras, name, strlen(name));
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::DestroyAllCameras()
{
    unsigned int i;
    for (i = 0; i < this->cameras.bucket_count; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(&this->cameras, i);
        while (it != NULL)
        {
            CE_DELETE (Camera*)((ce_key_value_pair*)it->data)->value;
            it = it->next;
        }
    }

    ce_hash_table_clear(&this->cameras);
}

//------------------------------------------------------------------------------
/**
*/
Light*
SceneManager::CreateLight(const char* name)
{
    Light* light = CE_NEW Light();

    ce_hash_table_insert(&this->lights, name, strlen(name), light);

    return light;
}

//------------------------------------------------------------------------------
/**
*/
Light*
SceneManager::GetLight(const char* name) const
{
    return (Light*)ce_hash_table_find(&this->lights, name, strlen(name));
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::DestroyAllLights()
{
    unsigned int i;
    for (i = 0; i < this->lights.bucket_count; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(&this->lights, i);
        while (it != NULL)
        {
            CE_DELETE (Light*)((ce_key_value_pair*)it->data)->value;
            it = it->next;
        }
    }

    ce_hash_table_clear(&this->lights);
}

//------------------------------------------------------------------------------
/**
*/
Entity*
SceneManager::CreateEntity(const char* meshName)
{
    Entity* entity = CE_NEW Entity(MeshManager::Instance()->Load(meshName));

    ce_dynamic_array_push_back(&this->entities, entity);

    return entity;
}

//------------------------------------------------------------------------------
/**
*/
SceneNode*
SceneManager::CreateSceneNode()
{
    SceneNode* sceneNode = CE_NEW SceneNode();
    sceneNode->SetParent(NULL);

    ce_dynamic_array_push_back(&this->sceneNodes, sceneNode);

    return sceneNode;
}

//------------------------------------------------------------------------------
/**
*/
SceneNode*
SceneManager::GetRootSceneNode()
{
    if (NULL == this->sceneRoot)
    {
        this->sceneRoot = CE_NEW SceneNode();
    }

    return this->sceneRoot;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::ClearScene()
{
    this->DestroyAllLights();

    this->GetRootSceneNode()->RemoveAllChildren();

    unsigned int i;
    for (i = 0; i < this->sceneNodes.size; ++i)
    {
        CE_DELETE (SceneNode*)ce_dynamic_array_get(&this->sceneNodes, i);
    }
    ce_dynamic_array_delete(&this->sceneNodes);

    for (i = 0; i < this->entities.size; ++i)
    {
        CE_DELETE(Entity*)ce_dynamic_array_get(&this->entities, i);
    }
    ce_dynamic_array_delete(&this->entities);
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::SetAmbientLight(unsigned int colour)
{
    this->ambientLight = colour;
    this->destRenderSystem->SetAmbientLight(this->ambientLight);
}

//------------------------------------------------------------------------------
/**
*/
const unsigned int
SceneManager::GetAmbientLight() const
{
    return this->ambientLight;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::SetShadowTechnique(ShadowTechnique technique)
{
    this->shadowTechnique = technique;

    if (SHADOWTYPE_TEXTURE_ADDITIVE == technique)
    {
        // setup texture shadowing
        if (this->shadowTextureConfigDirty)
        {
            this->shadowCamera = CE_NEW Camera();
            this->shadowCamera->SetAspectRatio(1.0f);

            this->shadowRenderTexture = CE_NEW RenderTexture();
            this->shadowRttPass = CE_NEW Pass(0);
            this->shadowRttPass->SetDepthCheckEnabled(false);
            this->shadowPass = CE_NEW Pass(0);
            TextureUnitState* tus = this->shadowPass->CreateTextureUnitState();
            tus->SetTextureAddressingMode(TextureUnitState::TAM_CLAMP, TextureUnitState::TAM_CLAMP);

#if __PSP__
            this->shadowRenderTexture->Create(256, 256, PF_A4R4G4B4);
            // fake GL_CLAMP_TO_BORDER by clearing the rendertarget with transparency and setting the viewport one pixel
            // smaller than the rendertarget on every side
            memset(this->shadowRenderTexture->GetBuffer(), 0xff, 131072);
            Viewport* vp = this->shadowRenderTexture->AddViewport(this->shadowCamera, 1, 1, 254, 254);
            vp->SetClearEveryFrame(true, FBT_COLOUR);
            vp->SetBackgroundColour(0xffffffff);
            // blending: fragmentcolor * 0xff888888 + framebufferpixelcolor * 0xff000000 (fragmentcolor = modelvertexcolor = 0xffffffff)
            this->shadowRttPass->SetSceneBlendingEnabled(true);
            this->shadowRttPass->SetSceneBlending(SBF_FIX, SBF_FIX);
            this->shadowRttPass->SetBlendingFixColors(0xff888888, 0xff000000);
            this->shadowPass->SetSceneBlendingEnabled(true);
            this->shadowPass->SetSceneBlending(SBF_SOURCE_COLOUR, SBF_ONE_MINUS_SOURCE_ALPHA);
            tus->SetTextureBlendOperation(LBT_COLOUR, LBO_REPLACE);
            tus->SetTextureMappingMode(TextureUnitState::TMM_TEXTURE_MATRIX);
            tus->SetTextureProjectionMappingMode(TextureUnitState::TPM_POSITION);
#endif

            this->shadowTexture = CE_NEW Texture();
            this->shadowTexture->SetFormat(this->shadowRenderTexture->GetFormat());
            this->shadowTexture->SetWidth(this->shadowRenderTexture->GetWidth());
            this->shadowTexture->SetHeight(this->shadowRenderTexture->GetHeight());
            this->shadowTexture->SetBuffer(this->shadowRenderTexture->GetBuffer());
            this->shadowTexture->SetSwizzleEnabled(false);
            tus->SetTexture(this->shadowTexture);

            this->shadowTextureProjScaleTrans = Matrix4::IDENTITY;
            this->shadowTextureProjScaleTrans[0][0] = 0.5f;
            this->shadowTextureProjScaleTrans[1][1] = -0.5f;
            this->shadowTextureProjScaleTrans[0][3] = 0.5f;
            this->shadowTextureProjScaleTrans[1][3] = 0.5f;

            this->shadowProjection = this->shadowTextureProjScaleTrans * (this->shadowCamera->GetProjectionMatrixRS() * this->shadowCamera->GetViewMatrix());

            this->shadowTextureConfigDirty = false;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
ShadowTechnique
SceneManager::GetShadowTechnique() const
{
    return this->shadowTechnique;
}

//------------------------------------------------------------------------------
/**
*/
bool
SceneManager::IsShadowTechniqueInUse() const
{
    return this->shadowTechnique != SHADOWTYPE_NONE;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::_RenderScene(Camera* camera, Viewport* vp)
{
    if (this->illuminationStage != IRS_RENDER_TO_TEXTURE)
    {
        // update transformation
        this->GetRootSceneNode()->_Update();

        // are we using any shadows at all?
        if (this->IsShadowTechniqueInUse() && !this->suppressRenderStateChanges)
        {
            this->PrepareShadowTextures();
        }
    }

    // fill renderqueues
    unsigned int sceneNodeIndex;
    for (sceneNodeIndex = 0; sceneNodeIndex < this->sceneNodes.size; ++sceneNodeIndex)
    {
        // for all scenenodes
        SceneNode* sceneNode = (SceneNode*)ce_dynamic_array_get(&this->sceneNodes, sceneNodeIndex);

        unsigned int entityIndex;
        for (entityIndex = 0; entityIndex < sceneNode->NumAttachedObjects(); ++entityIndex)
        {
            // for all attached entities
            Entity* entity = sceneNode->GetAttachedObject((unsigned short)entityIndex);

            // apply frustum culling
            // for correct results with non-uniform scaled scenenodes we have
            // to take the maximum value of the three vector elements of the
            // scaling vector as scale factor for the sphere radius
            const Vector3& nodeScale = sceneNode->GetScale();
            float maxScale = nodeScale.x;
            if (nodeScale.y > maxScale) {maxScale = nodeScale.y;}
            if (nodeScale.z > maxScale) {maxScale = nodeScale.z;}
            if (!camera->IsVisible(sceneNode->_GetDerivedPosition(), entity->GetMesh()->GetBoundingSphereRadius() * maxScale))
            {
                continue;
            }

            if (this->IsShadowTechniqueInUse() && this->illuminationStage == IRS_RENDER_TO_TEXTURE && !entity->GetCastShadows())
            {
                continue;
            }

            // update animation
            if (entity->HasVertexAnimation() && this->illuminationStage != IRS_RENDER_TO_TEXTURE)
            {
                entity->UpdateAnimation();
            }

            unsigned int subEntityIndex;
            for (subEntityIndex = 0; subEntityIndex < entity->GetNumSubEntities(); ++subEntityIndex)
            {
                // for all subentities
                SubEntity* subEntity = entity->GetSubEntity(subEntityIndex);

                if (!subEntity->IsVisible())
                {
                    continue;
                }

                Material* material = subEntity->GetMaterial();
                if (NULL == material)
                {
                    continue;
                }

                // add to shadow caster queue
                if (this->IsShadowTechniqueInUse() && this->illuminationStage == IRS_RENDER_TO_TEXTURE)
                {
                    this->renderQueueOpaque.AddRenderable(subEntity, this->shadowRttPass);
                    continue;
                }

                unsigned int passIndex;
                for (passIndex = 0; passIndex < material->GetNumPasses(); ++passIndex)
                {
                    // for all passes
                    Pass* pass = material->GetPass((unsigned short)passIndex);

                    if (pass->GetSceneBlendingEnabled())
                    {
                        // add to transparency queue
                        this->renderQueueTransparent.AddRenderable(subEntity, pass);
                    }
                    else
                    {
                        // add to opaque queue
                        this->renderQueueOpaque.AddRenderable(subEntity, pass);
                    }
                }

                // add to shadow receiver queue
                if (this->IsShadowTechniqueInUse() && entity->GetReceivesShadows() && this->illuminationStage != IRS_RENDER_TO_TEXTURE && !this->suppressRenderStateChanges)
                {
                    this->renderQueueShadowReceiver.AddRenderable(subEntity, NULL);
                }
            }
        }
    }

    // render queues
    this->destRenderSystem->_BeginFrame();

    this->destRenderSystem->_SetRenderTarget(vp->GetTarget());
    this->destRenderSystem->_SetViewport(vp);

    this->destRenderSystem->_SetProjectionMatrix(camera->GetProjectionMatrixRS());
    this->destRenderSystem->_SetViewMatrix(camera->GetViewMatrix());

    this->destRenderSystem->_UseLights(&this->lights);

    if (this->IsShadowTechniqueInUse() && this->illuminationStage == IRS_RENDER_TO_TEXTURE)
    {
        this->_RenderQueueGroupObjects(&this->renderQueueOpaque);
    }
    else
    {
        this->_RenderQueueGroupObjects(&this->renderQueueOpaque);
        this->_RenderQueueGroupObjects(&this->renderQueueTransparent);

        if (this->IsShadowTechniqueInUse() && !this->suppressRenderStateChanges)
        {
            this->_RenderTextureShadowReceiverQueueGroupObjects(&this->renderQueueShadowReceiver);
        }
    }

    this->destRenderSystem->_EndFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::_SetPass(Pass* pass)
{
    this->destRenderSystem->_SetPass(pass);
}

//---------------------------------------------------------------------
void
SceneManager::_SuppressRenderStateChanges(bool suppress)
{
    this->suppressRenderStateChanges = suppress;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::PrepareShadowTextures()
{
    this->illuminationStage = IRS_RENDER_TO_TEXTURE;

    unsigned int i;
    for (i = 0; i < this->lights.bucket_count; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(&this->lights, i);
        while (it != NULL)
        {
            Light* light = (graphics::Light*)((ce_key_value_pair*)it->data)->value;
            if (Light::LT_SPOTLIGHT == light->GetType())
            {
                this->shadowCamera->SetPosition(light->GetPosition());
                this->shadowCamera->SetDirection(light->GetDirection());
                this->shadowProjection = this->shadowTextureProjScaleTrans * (this->shadowCamera->GetProjectionMatrixRS() * this->shadowCamera->GetViewMatrix());
                break;
            }
            it = it->next;
        }
    }

    this->shadowRenderTexture->Update();

    this->illuminationStage = IRS_NONE;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::_RenderQueueGroupObjects(QueuedRenderableCollection* queue)
{
    Pass* lastPass = NULL;

    unsigned short numRenderablePasses = queue->GetNumRenderablePasses();
    unsigned short i;
    for (i = 0; i < numRenderablePasses; ++i)
    {
        RenderablePass* renderablePass = queue->GetRenderablePass(i);

        // apply pass
        if (!this->suppressRenderStateChanges)
        {
            Pass* pass = renderablePass->pass;
            if (pass != lastPass)
            {
                this->destRenderSystem->_SetPass(pass);
            }
            lastPass = pass;
        }

        // render entity
        SubEntity* renderable = renderablePass->renderable;
        this->destRenderSystem->_SetWorldMatrix(renderable->parentEntity->parentNode->_GetFullTransform());
        this->destRenderSystem->_Render(renderable);
    }

    queue->Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::_RenderTextureShadowReceiverQueueGroupObjects(QueuedRenderableCollection* queue)
{
    this->destRenderSystem->_SetPass(this->shadowPass);

    unsigned short numRenderablePasses = queue->GetNumRenderablePasses();
    unsigned short i;
    for (i = 0; i < numRenderablePasses; ++i)
    {
        SubEntity* renderable = queue->GetRenderablePass(i)->renderable;

        this->destRenderSystem->_SetWorldMatrix(renderable->parentEntity->parentNode->_GetFullTransform());
        this->destRenderSystem->_SetTextureMatrix(this->shadowProjection * renderable->parentEntity->parentNode->_GetFullTransform());
        this->destRenderSystem->_Render(renderable);
    }

    queue->Clear();
}

} // namespace graphics
} // namespace chrissly