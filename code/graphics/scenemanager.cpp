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
    renderQueuesEndedCallback(NULL),
    suppressRenderStateChanges(false),
    illuminationStage(IRS_NONE),
    shadowTechnique(SHADOWTYPE_NONE),
    shadowTextureConfigDirty(true),
    shadowTexture(NULL),
    shadowRenderTexture(NULL),
    shadowCamera(NULL),
    shadowRttPass(NULL),
    shadowRttMorphAnimPass(NULL),
    shadowPass(NULL)
{
    Singleton = this;

    ce_hash_table_init(&this->cameras, 2U);
    ce_hash_table_init(&this->lights, 4U);
    ce_dynamic_array_init(&this->entities, 256U);
    ce_dynamic_array_init(&this->sceneNodes, 256U);

    this->renderQueueOpaque.Initialise(512U);
    this->renderQueueTransparent.Initialise(256U);
    this->renderQueueShadowReceiver.Initialise(256U);

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
        CE_DELETE this->shadowRttMorphAnimPass;
        CE_DELETE this->shadowPass;
    }
}

//------------------------------------------------------------------------------
/**
*/
Camera* const
SceneManager::CreateCamera(const char* const name)
{
    Camera* camera = CE_NEW Camera();

    ce_hash_table_insert(&this->cameras, name, strlen(name), camera);

    return camera;
}

//------------------------------------------------------------------------------
/**
*/
Camera* const
SceneManager::GetCamera(const char* const name) const
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
    for (i = 0U; i < this->cameras.bucket_count; ++i)
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
Light* const
SceneManager::CreateLight(const char* const name)
{
    Light* light = CE_NEW Light();

    ce_hash_table_insert(&this->lights, name, strlen(name), light);

    return light;
}

//------------------------------------------------------------------------------
/**
*/
Light* const
SceneManager::GetLight(const char* const name) const
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
    for (i = 0U; i < this->lights.bucket_count; ++i)
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
Entity* const
SceneManager::CreateEntity(const char* const meshName)
{
    Entity* entity = CE_NEW Entity(MeshManager::Instance()->Load(meshName));

    ce_dynamic_array_push_back(&this->entities, entity);

    return entity;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::DestroyEntity(Entity* const entity)
{
    CE_ASSERT(entity != NULL, "SceneManager::DestroyEntity(): invalid pointer passed");
    ce_dynamic_array_erase(&this->entities, entity);
    CE_DELETE entity;
}

//------------------------------------------------------------------------------
/**
*/
SceneNode* const
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
void
SceneManager::DestroySceneNode(SceneNode* const node)
{
    CE_ASSERT(node != NULL, "SceneManager::DestroySceneNode(): invalid pointer passed");
    ce_dynamic_array_erase(&this->sceneNodes, node);
    CE_DELETE node;
}

//------------------------------------------------------------------------------
/**
*/
SceneNode* const
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

    SceneNode* root = this->GetRootSceneNode();
    root->RemoveAllChildren();
    root->DetachAllObjects();

    unsigned int i;
    for (i = 0U; i < this->sceneNodes.size; ++i)
    {
        CE_DELETE (SceneNode*)ce_dynamic_array_get(&this->sceneNodes, i);
    }
    ce_dynamic_array_delete(&this->sceneNodes);

    for (i = 0U; i < this->entities.size; ++i)
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

            this->shadowTextureProjScaleTrans = Matrix4::IDENTITY;
            this->shadowTextureProjScaleTrans[0U][0U] = 0.5f;
            this->shadowTextureProjScaleTrans[1U][1U] = -0.5f;
            this->shadowTextureProjScaleTrans[0U][3U] = 0.5f;
            this->shadowTextureProjScaleTrans[1U][3U] = 0.5f;
            this->shadowProjection = this->shadowTextureProjScaleTrans * (this->shadowCamera->GetProjectionMatrix() * this->shadowCamera->GetViewMatrix());

            this->shadowRenderTexture = CE_NEW RenderTexture();
            this->shadowRttPass = CE_NEW Pass(0U);
            this->shadowRttPass->SetCullingMode(CULL_NONE);
            this->shadowRttMorphAnimPass = CE_NEW Pass(0U);
            this->shadowRttMorphAnimPass->SetCullingMode(CULL_NONE);
            this->shadowPass = CE_NEW Pass(0U);
            this->shadowPass->SetSceneBlendingEnabled(true);
            this->shadowPass->SetSceneBlending(SBF_DEST_COLOUR, SBF_ZERO);
            TextureUnitState* tus = this->shadowPass->CreateTextureUnitState();
            tus->SetTextureAddressingMode(TextureUnitState::TAM_BORDER, TextureUnitState::TAM_BORDER);

#if __CE_PSP__
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
            this->shadowRttPass->SetDepthCheckEnabled(false);
            tus->SetTextureMappingMode(TextureUnitState::TMM_TEXTURE_MATRIX);
            tus->SetTextureProjectionMappingMode(TextureUnitState::TPM_POSITION);
#elif __CE_D3D11__
            this->shadowRenderTexture->Create(512, 512, PF_R8G8B8A8);
            Viewport* vp = this->shadowRenderTexture->AddViewport(this->shadowCamera, 0, 0, 511, 511);
            vp->SetClearEveryFrame(true, FBT_COLOUR);
            vp->SetBackgroundColour(0xffffffff);
            this->shadowRttPass->SetGpuProgram(this->destRenderSystem->GetDefaultShadowCasterGpuProgram());
            this->shadowRttMorphAnimPass->SetGpuProgram(this->destRenderSystem->GetDefaultShadowCasterMorphAnimGpuProgram());
            this->shadowPass->SetGpuProgram(this->destRenderSystem->GetDefaultShadowReceiverGpuProgram());
#endif

            this->shadowTexture = CE_NEW Texture(this->shadowRenderTexture);
            tus->SetTexture(this->shadowTexture);

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
SceneManager::_RenderScene(Camera* const camera, Viewport* const vp)
{
    if (this->illuminationStage != IRS_RENDER_TO_TEXTURE)
    {
        // update transformation
        this->GetRootSceneNode()->_Update(false);

        // update shadowmap if shadows are enabled
        if (this->IsShadowTechniqueInUse() && !this->suppressRenderStateChanges)
        {
            this->PrepareShadowTextures();
        }
    }

    // fill renderqueues
    unsigned int sceneNodeIndex;
    for (sceneNodeIndex = 0U; sceneNodeIndex < this->sceneNodes.size; ++sceneNodeIndex)
    {
        // for all scenenodes
        SceneNode* sceneNode = (SceneNode*)ce_dynamic_array_get(&this->sceneNodes, sceneNodeIndex);

        unsigned int entityIndex;
        for (entityIndex = 0U; entityIndex < sceneNode->NumAttachedObjects(); ++entityIndex)
        {
            // for all attached entities
            Entity* entity = sceneNode->GetAttachedObject((unsigned short)entityIndex);

            if (!entity->IsVisible())
            {
                continue;
            }

            if (this->IsShadowTechniqueInUse() && this->illuminationStage == IRS_RENDER_TO_TEXTURE && !entity->GetCastShadows())
            {
                continue;
            }

            // apply frustum culling
            // for correct results with non-uniform scaled scenenodes we have to take the maximum value
            // of the three vector elements of the scaling vector as scale factor for the sphere radius
            if (!camera->IsVisible(sceneNode->_GetDerivedPosition(), entity->GetMesh()->GetBoundingSphereRadius() * sceneNode->GetScale().GetAbsMax()))
            {
                continue;
            }

            // update animation
            if (entity->HasVertexAnimation() && (!this->IsShadowTechniqueInUse() ||
                (this->IsShadowTechniqueInUse() && this->illuminationStage == IRS_RENDER_TO_TEXTURE) ||
                (this->IsShadowTechniqueInUse() && this->illuminationStage != IRS_RENDER_TO_TEXTURE && !entity->GetCastShadows())))
            {
                entity->UpdateAnimation();
            }

            unsigned int subEntityIndex;
            for (subEntityIndex = 0U; subEntityIndex < entity->GetNumSubEntities(); ++subEntityIndex)
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
#if __CE_D3D11__
                    this->renderQueueOpaque.AddRenderable(subEntity, (VAT_MORPH == subEntity->GetSubMesh()->GetVertexAnimationType()) ? this->shadowRttMorphAnimPass : this->shadowRttPass);
#else
                    this->renderQueueOpaque.AddRenderable(subEntity, this->shadowRttPass);
#endif
                    continue;
                }

                unsigned int passIndex;
                for (passIndex = 0U; passIndex < material->GetNumPasses(); ++passIndex)
                {
                    // for all passes
                    Pass* pass = material->GetPass((unsigned short)passIndex);

                    if (pass->GetSceneBlendingEnabled())
                    {
                        // add to transparency queue
                        subEntity->distanceToCamera = (camera->GetPosition() - sceneNode->GetPosition()).Length();
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
    this->destRenderSystem->BeginFrame();

    this->destRenderSystem->SetRenderTarget(vp->GetTarget());
    this->destRenderSystem->SetViewport(vp);

    this->destRenderSystem->SetProjectionMatrix(camera->GetProjectionMatrix());
    this->destRenderSystem->SetViewMatrix(camera->GetViewMatrix());

    this->destRenderSystem->ProcessLights(&this->lights);

    if (this->IsShadowTechniqueInUse() && this->illuminationStage == IRS_RENDER_TO_TEXTURE)
    {
        this->RenderQueueGroupObjects(&this->renderQueueOpaque);
    }
    else
    {
        this->RenderQueueGroupObjects(&this->renderQueueOpaque);
        // Sort transparent renderables by distance to camera from furthest to nearest, cause blending is usually order dependent.
        // Traditional alpha blending (BlendFunc(SRC_ALPHA, ONE_MINUS_SRC_ALPHA)) for instance is order dependent cause
        // the subtraction is not commutative, hence the order in which the values are subtracted from each other matters.
        this->renderQueueTransparent.Sort();
        this->RenderQueueGroupObjects(&this->renderQueueTransparent);

        if (this->IsShadowTechniqueInUse() && !this->suppressRenderStateChanges)
        {
            this->RenderTextureShadowReceiverQueueGroupObjects(&this->renderQueueShadowReceiver);
        }

        if (this->renderQueuesEndedCallback != NULL)
        {
            this->renderQueuesEndedCallback();
        }
    }

    this->destRenderSystem->EndFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::_SetPass(Pass* const pass)
{
    this->destRenderSystem->SetPass(pass);
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::_SuppressRenderStateChanges(bool suppress)
{
    this->suppressRenderStateChanges = suppress;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::_SetRenderQueuesEndedCallback(RenderQueuesEndedCallback callback)
{
    this->renderQueuesEndedCallback = callback;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::PrepareShadowTextures()
{
    bool shadowCameraDirty = true;
    unsigned int i;
    for (i = 0U; i < this->lights.bucket_count && shadowCameraDirty; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(&this->lights, i);
        while (it != NULL && shadowCameraDirty)
        {
            Light* light = (graphics::Light*)((ce_key_value_pair*)it->data)->value;
            if (Light::LT_SPOTLIGHT == light->GetType())
            {
                this->shadowCamera->SetPosition(light->GetPosition());
                this->shadowCamera->SetDirection(light->GetDirection());
                this->shadowProjection = this->shadowTextureProjScaleTrans * (this->shadowCamera->GetProjectionMatrix() * this->shadowCamera->GetViewMatrix());
                shadowCameraDirty = false;
            }
            it = it->next;
        }
    }

    this->illuminationStage = IRS_RENDER_TO_TEXTURE;

    this->shadowRenderTexture->Update();

    this->illuminationStage = IRS_NONE;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::RenderQueueGroupObjects(RenderQueue* const queue)
{
    Pass* lastPass = NULL;

    unsigned short numRenderablePasses = queue->GetNumRenderablePasses();
    unsigned short i;
    for (i = 0U; i < numRenderablePasses; ++i)
    {
        RenderablePass* renderablePass = queue->GetRenderablePass(i);

        // apply pass
        if (!this->suppressRenderStateChanges)
        {
            Pass* pass = renderablePass->pass;
            if (pass != lastPass)
            {
                this->destRenderSystem->SetPass(pass);
                lastPass = pass;
            }
        }

        // render entity
        SubEntity* renderable = renderablePass->renderable;
        this->destRenderSystem->SetWorldMatrix(renderable->parentEntity->parentNode->_GetFullTransform());
        this->destRenderSystem->Render(renderable);
    }

    queue->Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::RenderTextureShadowReceiverQueueGroupObjects(RenderQueue* const queue)
{
    this->destRenderSystem->SetPass(this->shadowPass);

    unsigned short numRenderablePasses = queue->GetNumRenderablePasses();
    unsigned short i;
    for (i = 0U; i < numRenderablePasses; ++i)
    {
        SubEntity* renderable = queue->GetRenderablePass(i)->renderable;

        this->destRenderSystem->SetWorldMatrix(renderable->parentEntity->parentNode->_GetFullTransform());
        this->destRenderSystem->SetTextureMatrix(this->shadowProjection * renderable->parentEntity->parentNode->_GetFullTransform());
        this->destRenderSystem->Render(renderable);
    }

    queue->Clear();
}

} // namespace graphics
} // namespace chrissly