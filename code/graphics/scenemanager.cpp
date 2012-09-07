//------------------------------------------------------------------------------
//  scenemanager.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "graphicssystem.h"
#include "scenemanager.h"
#include "meshmanager.h"
#include "renderqueuesortinggrouping.h"
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
    movableObjectCollectionMap(NULL),
    sceneNodes(NULL),
    sceneRoot(NULL),
    ambientLight(0x00000000),
    renderQueueOpaque(NULL),
    renderQueueTransparent(NULL),
    renderQueueShadowCaster(NULL),
    renderQueueShadowReceiver(NULL),
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
SceneManager::GetCamera(const char* name)
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
    for (i = 0; i < this->cameras.capacity; i++)
    {
        Chain* chain = (Chain*)DynamicArrayGet(&this->cameras.entries, i);
        LinkedList* it = chain->list;
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
Entity*
SceneManager::CreateEntity(const char* meshName)
{
    Entity* entity = CE_NEW Entity("", MeshManager::Instance()->Load(meshName));

    this->movableObjectCollectionMap = linkedlistAdd(&this->movableObjectCollectionMap, entity);
    this->movableObjectCollectionMap->data = entity;

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

    this->sceneNodes = linkedlistAdd(&this->sceneNodes, sceneNode);
    this->sceneNodes->data = sceneNode;

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
    LinkedList* it = this->movableObjectCollectionMap;
    while (it != NULL)
    {
        LinkedList* node = it;

        CE_DELETE (Entity*)node->data;

        it = it->next;
        linkedlistRemove(node);
    }
    this->movableObjectCollectionMap = NULL;
    
    this->sceneRoot->RemoveAllChildren();
    
    it = this->sceneNodes;
    while (it != NULL)
    {
        LinkedList* node = it;

        CE_DELETE (SceneNode*)node->data;

        it = it->next;
        linkedlistRemove(node);
    }
    this->sceneNodes = NULL;
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
            this->shadowRenderTexture->Create(256, 256, PF_A4R4G4B4);
            Viewport* vp = this->shadowRenderTexture->AddViewport(this->shadowCamera, 128, 128, 256, 256);
            vp->SetBackgroundColour(0xffffffff);
            vp->SetClearEveryFrame(true, FBT_COLOUR);

            this->shadowTexture = CE_NEW Texture();
            this->shadowTexture->SetFormat(this->shadowRenderTexture->GetFormat());
            this->shadowTexture->SetWidth(this->shadowRenderTexture->GetWidth());
            this->shadowTexture->SetHeight(this->shadowRenderTexture->GetHeight());
            this->shadowTexture->SetBuffer(this->shadowRenderTexture->GetBuffer());
            this->shadowTexture->SetSwizzleEnabled(false);
 
            // FragmentColor * 0xff888888 + FrameBufferPixelColor * 0xff000000 (FragmentColor = ModelVertexColor = White)
            this->shadowRttPass = CE_NEW Pass(0);
            this->shadowRttPass->SetSceneBlendingEnabled(true);
            this->shadowRttPass->SetSceneBlending(SBF_FIX, SBF_FIX);
            this->shadowRttPass->SetBlendingFixColors(0xff888888, 0xff000000);
            this->shadowRttPass->SetDepthCheckEnabled(false); 

            this->shadowPass = CE_NEW Pass(0);
            this->shadowPass->SetSceneBlendingEnabled(true);
            this->shadowPass->SetSceneBlending(SBF_SOURCE_COLOUR, SBF_ONE_MINUS_SOURCE_ALPHA);
            
            TextureUnitState* tus = this->shadowPass->CreateTextureUnitState();
            tus->SetTextureBlendOperation(LBT_COLOUR, LBO_REPLACE);
            tus->SetTextureAddressingMode(TextureUnitState::TAM_CLAMP, TextureUnitState::TAM_CLAMP);
            tus->SetTextureMappingMode(TextureUnitState::TMM_TEXTURE_MATRIX);
            tus->SetTextureProjectionMappingMode(TextureUnitState::TPM_POSITION);
            tus->SetTexture(this->shadowTexture);

            this->shadowCamera->SetPosition(4.0f, 4.0f, -4.0f);
            this->shadowCamera->Yaw(2.14f);
            this->shadowCamera->Pitch(-1.0f);

            this->shadowTextureProjScaleTrans = Matrix4::IDENTITY;
            this->shadowTextureProjScaleTrans[0][0] = 0.5f;
            this->shadowTextureProjScaleTrans[1][1] = -0.5f;
            this->shadowTextureProjScaleTrans[0][3] = 0.5f;
            this->shadowTextureProjScaleTrans[1][3] = 0.5f;

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
SceneManager::_RenderScene(Camera *camera, Viewport *vp)
{ 
    if (this->illuminationStage != IRS_RENDER_TO_TEXTURE)
    {
        // are we using any shadows at all?
        if (this->IsShadowTechniqueInUse())
        {
            this->PrepareShadowTextures();
        }
    }

    this->destRenderSystem->_SetRenderTarget(vp->GetTarget());
    this->destRenderSystem->_SetViewport(vp);
    
    this->destRenderSystem->_SetProjectionMatrix(camera->GetProjectionMatrixRS());
    this->destRenderSystem->_SetViewMatrix(camera->GetViewMatrix());

    // update transformation
    this->GetRootSceneNode()->_Update();
    
    // fill renderqueues
    LinkedList* sceneNodeIt = this->sceneNodes;
    while (sceneNodeIt != NULL)
    {
        // for all scenenodes
        SceneNode* sceneNode = (SceneNode*)sceneNodeIt->data;
        
        unsigned int entityIndex;
        for (entityIndex = 0; entityIndex < sceneNode->NumAttachedObjects(); entityIndex++)
        {
            // for all attached entities
            Entity* entity = sceneNode->GetAttachedObject(entityIndex);
            
            if (this->IsShadowTechniqueInUse() && this->illuminationStage == IRS_RENDER_TO_TEXTURE && !entity->GetCastShadows()) continue;
            
            // ----- update animation here -----
            
            unsigned int subEntityIndex;
            for (subEntityIndex = 0; subEntityIndex < entity->GetNumSubEntities(); subEntityIndex++)
            {
                // for all subentities
                SubEntity* subEntity = entity->GetSubEntity(subEntityIndex);
                
                if (!subEntity->IsVisible()) continue;
                
                Material* material = subEntity->GetMaterial();
                if (NULL == material) continue;
 
                // add to shadow caster queue
                if (this->IsShadowTechniqueInUse() && this->illuminationStage == IRS_RENDER_TO_TEXTURE)
                {
                    RenderablePass* renderablePass = CE_NEW RenderablePass(subEntity, this->shadowRttPass);
                
                    this->renderQueueShadowCaster = linkedlistAdd(&this->renderQueueShadowCaster, renderablePass);
                    this->renderQueueShadowCaster->data = renderablePass;
                    
                    continue;
                }
                
                unsigned int passIndex;
                for (passIndex = 0; passIndex < material->GetNumPasses(); passIndex++)
                {
                    // for all passes
                    Pass* pass = material->GetPass(passIndex);
                    
                    RenderablePass* renderablePass = CE_NEW RenderablePass(subEntity, pass);
                    
                    if (pass->GetSceneBlendingEnabled())
                    {
                        // add to transparency queue
                        this->renderQueueTransparent = linkedlistAdd(&this->renderQueueTransparent, renderablePass);
                        this->renderQueueTransparent->data = renderablePass;
                    }
                    else
                    {
                        // add to opaque queue
                        this->renderQueueOpaque = linkedlistAdd(&this->renderQueueOpaque, renderablePass);
                        this->renderQueueOpaque->data = renderablePass;
                    }
                }
                
                // add to shadow receiver queue
                if (this->IsShadowTechniqueInUse() && subEntity->parentEntity->GetReceivesShadows() && this->illuminationStage != IRS_RENDER_TO_TEXTURE)
                {
                    RenderablePass* renderablePass = CE_NEW RenderablePass(subEntity, NULL);
                
                    this->renderQueueShadowReceiver = linkedlistAdd(&this->renderQueueShadowReceiver, renderablePass);
                    this->renderQueueShadowReceiver->data = renderablePass;
                }
            }
        }
        
        sceneNodeIt = sceneNodeIt->next;
    }
    
    // render queues
    if (this->IsShadowTechniqueInUse() && this->illuminationStage == IRS_RENDER_TO_TEXTURE)
    {
        this->_RenderQueueGroupObjects(this->renderQueueShadowCaster);
        this->_ClearRenderQueue(this->renderQueueShadowCaster);
        this->renderQueueShadowCaster = NULL;
    }
    else
    {    
        this->_RenderQueueGroupObjects(this->renderQueueOpaque);
        this->_RenderQueueGroupObjects(this->renderQueueTransparent); 
        
        if (this->IsShadowTechniqueInUse())
        {
            this->_RenderTextureShadowReceiverQueueGroupObjects(this->renderQueueShadowReceiver);
            this->_ClearRenderQueue(this->renderQueueShadowReceiver);
            this->renderQueueShadowReceiver = NULL;
        }
        
        this->_ClearRenderQueue(this->renderQueueTransparent);
        this->renderQueueTransparent = NULL;
        
        this->_ClearRenderQueue(this->renderQueueOpaque);
        this->renderQueueOpaque = NULL;
    }
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
SceneManager::_RenderQueueGroupObjects(LinkedList* queue)
{
    Pass* lastPass = NULL;

    LinkedList* queueIt = queue;
    while (queueIt != NULL)
    {
        RenderablePass* renderablePass = (RenderablePass*)queueIt->data;
 
        // apply pass
        Pass* pass = renderablePass->pass;
        if (pass != lastPass)
        {
            this->_SetPass(pass);
        }
        lastPass = pass;
 
        // render entity
        SubEntity* renderable = renderablePass->renderable;
        this->destRenderSystem->_SetWorldMatrix(renderable->parentEntity->parentNode->_GetFullTransform());
        this->destRenderSystem->_Render(renderable);
 
        queueIt = queueIt->next;
    } 
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::_RenderTextureShadowReceiverQueueGroupObjects(LinkedList* queue)
{
    this->_SetPass(this->shadowPass);

    LinkedList* queueIt = queue;
    while (queueIt != NULL)
    {
        SubEntity* renderable = ((RenderablePass*)queueIt->data)->renderable;
        this->destRenderSystem->_SetWorldMatrix(renderable->parentEntity->parentNode->_GetFullTransform());
        this->destRenderSystem->_SetTextureMatrix(this->shadowProjection * renderable->parentEntity->parentNode->_GetFullTransform());
        this->destRenderSystem->_Render(renderable);

        queueIt = queueIt->next;
    } 
}

//------------------------------------------------------------------------------
/**
*/
void
SceneManager::_ClearRenderQueue(LinkedList* queueIt)
{
    while (queueIt != NULL)
    {
        LinkedList* node = queueIt;

        CE_DELETE (RenderablePass*)node->data;

        queueIt = queueIt->next;
        linkedlistRemove(node);
    }
}

} // namespace graphics
} // namespace chrissly