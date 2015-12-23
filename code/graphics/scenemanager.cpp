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
    entities(NULL),
    sceneNodes(NULL),
    sceneRoot(NULL),
    ambientLight(0x00000000),
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

    HashTableInit(&this->cameras, 2);
    HashTableInit(&this->lights, 4);

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

    HashTableInsert(&this->cameras, name, camera);

    return camera;
}

//------------------------------------------------------------------------------
/**
*/
Camera*
SceneManager::GetCamera(const char* name) const
{
    return (Camera*)HashTableFind(&this->cameras, name);
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::DestroyAllCameras()
{
    unsigned int i;
    for (i = 0; i < this->cameras.capacity; ++i)
    {
        LinkedList* it = ((Chain*)DynamicArrayGet(&this->cameras.entries, i))->list;
        while (it != NULL)
        {
            CE_DELETE (Camera*)((KeyValuePair*)it->data)->value;
            it = it->next;
        }
    }

    HashTableClear(&this->cameras);
}

//------------------------------------------------------------------------------
/**
*/
Light*
SceneManager::CreateLight(const char* name)
{
    Light* light = CE_NEW Light();

    HashTableInsert(&this->lights, name, light);

    return light;
}

//------------------------------------------------------------------------------
/**
*/
Light*
SceneManager::GetLight(const char* name) const
{
    return (Light*)HashTableFind(&this->lights, name);
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::DestroyAllLights()
{
    unsigned int i;
    for (i = 0; i < this->lights.capacity; ++i)
    {
        LinkedList* it = ((Chain*)DynamicArrayGet(&this->lights.entries, i))->list;
        while (it != NULL)
        {
            CE_DELETE (Light*)((KeyValuePair*)it->data)->value;
            it = it->next;
        }
    }

    HashTableClear(&this->lights);
}

//------------------------------------------------------------------------------
/**
*/
Entity*
SceneManager::CreateEntity(const char* meshName)
{
    Entity* entity = CE_NEW Entity("", MeshManager::Instance()->Load(meshName));

    linkedlistAdd(&this->entities, entity);

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

    linkedlistAdd(&this->sceneNodes, sceneNode);

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
    LinkedList* it = this->entities;
    while (it != NULL)
    {
        LinkedList* node = it;
        CE_DELETE (Entity*)node->data;
        it = it->next;
        linkedlistRemove(node);
    }
    this->entities = NULL;

    this->GetRootSceneNode()->RemoveAllChildren();

    it = this->sceneNodes;
    while (it != NULL)
    {
        LinkedList* node = it;
        CE_DELETE (SceneNode*)node->data;
        it = it->next;
        linkedlistRemove(node);
    }
    this->sceneNodes = NULL;

    this->DestroyAllLights();
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

            this->shadowCamera->SetPosition(4.0f, 4.0f, -4.0f);
            this->shadowCamera->Yaw(2.14f);
            this->shadowCamera->Pitch(-1.0f);

            this->shadowProjection = this->shadowCamera->GetProjectionMatrixRS() * this->shadowCamera->GetViewMatrix();
            this->shadowProjection = this->shadowTextureProjScaleTrans * this->shadowProjection;
 
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
        if (this->IsShadowTechniqueInUse())
        {
            this->PrepareShadowTextures();
        }
    }

    // fill renderqueues
    LinkedList* sceneNodeIt = this->sceneNodes;
    while (sceneNodeIt != NULL)
    {
        // for all scenenodes
        SceneNode* sceneNode = (SceneNode*)sceneNodeIt->data;

        unsigned int entityIndex;
        for (entityIndex = 0; entityIndex < sceneNode->NumAttachedObjects(); ++entityIndex)
        {
            // for all attached entities
            Entity* entity = sceneNode->GetAttachedObject(entityIndex);
            
            if (this->IsShadowTechniqueInUse() && this->illuminationStage == IRS_RENDER_TO_TEXTURE && !entity->GetCastShadows()) continue;

            // update animation
            if (entity->HasVertexAnimation() && this->illuminationStage != IRS_RENDER_TO_TEXTURE) entity->UpdateAnimation();

            unsigned int subEntityIndex;
            for (subEntityIndex = 0; subEntityIndex < entity->GetNumSubEntities(); ++subEntityIndex)
            {
                // for all subentities
                SubEntity* subEntity = entity->GetSubEntity(subEntityIndex);
                
                if (!subEntity->IsVisible()) continue;
                
                Material* material = subEntity->GetMaterial();
                if (NULL == material) continue;

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
                    Pass* pass = material->GetPass(passIndex);

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
                if (this->IsShadowTechniqueInUse() && entity->GetReceivesShadows() && this->illuminationStage != IRS_RENDER_TO_TEXTURE)
                {
                    this->renderQueueShadowReceiver.AddRenderable(subEntity, NULL);
                }
            }
        }

        sceneNodeIt = sceneNodeIt->next;
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

        if (this->IsShadowTechniqueInUse())
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

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::PrepareShadowTextures()
{
    this->illuminationStage = IRS_RENDER_TO_TEXTURE;

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
        Pass* pass = renderablePass->pass;
        if (pass != lastPass)
        {
            this->destRenderSystem->_SetPass(pass);
        }
        lastPass = pass;

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