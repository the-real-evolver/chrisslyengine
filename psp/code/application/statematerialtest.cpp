//------------------------------------------------------------------------------
//  statematerialtest.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "statematerialtest.h"
#include "stateanimationtest.h"
#include "statemanager.h"
#include "graphicssystem.h"
#include "scenemanager.h"
#include <psputils.h>

namespace application
{

using namespace chrissly::graphics;
using namespace chrissly::core;

StateMaterialTest* StateMaterialTest::Singleton = NULL;

//------------------------------------------------------------------------------
/**
*/
StateMaterialTest::StateMaterialTest()
{
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
StateMaterialTest::~StateMaterialTest()
{
    Singleton = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
StateMaterialTest::Enter()
{
    sceCtrlSetSamplingCycle(100);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    // solid material
    this->solidMaterial = new Material();
    Texture* tex = TextureManager::Instance()->Load("gothic_solid.tex");
    tex->SetFormat(PF_R5G6B5);
    tex->SetWidth(512);
    tex->SetHeight(512);
    Pass* pass = this->solidMaterial->CreatePass();
    pass->SetFog(FOG_LINEAR, 0xff0080ff, 0.0f, 50.0f);
    TextureUnitState* tus = pass->CreateTextureUnitState();
    tus->SetTexture(tex);
    tus->SetTextureBlendOperation(LBT_COLOUR, LBO_REPLACE);
    
    // transparent material
    this->alphaMaterial = new Material();
    tex = TextureManager::Instance()->Load("gothic_alpha.tex");
    tex->SetFormat(PF_A4R4G4B4);
    tex->SetWidth(512);
    tex->SetHeight(512);
    pass = this->alphaMaterial->CreatePass();
    pass->SetSceneBlendingEnabled(true);
    pass->SetSceneBlending(SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
    pass->SetCullingMode(CULL_NONE);
    pass->SetFog(FOG_LINEAR, 0xff0080ff, 0.0f, 50.0f);
    tus = pass->CreateTextureUnitState();
    tus->SetTexture(tex);
    tus->SetTextureBlendOperation(LBT_ALPHA, LBO_REPLACE);
    
    // create entity and set materials
    this->gothEntity = SceneManager::Instance()->CreateEntity("gothic_woman.mesh");
    this->gothEntity->SetCastShadows(true);
    this->gothEntity->GetSubEntity(1)->SetMaterial(this->solidMaterial);
    this->gothEntity->GetSubEntity(0)->SetMaterial(this->alphaMaterial);
    this->gothEntity->GetSubEntity(2)->SetMaterial(this->alphaMaterial);
    
    // create scenenode
    this->gothSceneNode = SceneManager::Instance()->GetRootSceneNode()->CreateChildSceneNode();
    this->gothSceneNode->SetScale(3.5f, 3.5f, 3.5f);
    this->gothSceneNode->SetPosition(0.0f, 0.1f, -1.4f);
    this->gothSceneNode->AttachObject(this->gothEntity);
    
    this->cubeMaterial = new Material();

    tex = TextureManager::Instance()->Load("floor.tex");
    tex->SetFormat(PF_R5G6B5);
    tex->SetWidth(256);
    tex->SetHeight(256);
    pass = this->cubeMaterial->CreatePass();
    tus = pass->CreateTextureUnitState();
    tus->SetTextureBlendOperation(LBT_COLOUR, LBO_REPLACE);
    tus->SetTexture(tex);

    tex = TextureManager::Instance()->Load("water.tex");
    tex->SetFormat(PF_R5G6B5);
    tex->SetWidth(256);
    tex->SetHeight(256);
    pass = this->cubeMaterial->CreatePass();
    pass->SetSceneBlendingEnabled(true);
    pass->SetSceneBlending(SBF_SOURCE_COLOUR, SBF_DEST_COLOUR);
    tus = pass->CreateTextureUnitState();
    tus->SetTexture(tex);
    tus->SetTextureBlendOperation(LBT_COLOUR, LBO_REPLACE);

    this->cubeEntity = SceneManager::Instance()->CreateEntity("cube.mesh");
    this->cubeEntity->GetSubEntity(0)->SetMaterial(this->cubeMaterial);
    this->cubeEntity->SetReceivesShadows(true);
    this->cubeSceneNode = SceneManager::Instance()->GetRootSceneNode()->CreateChildSceneNode();
    this->cubeSceneNode->SetScale(10.0f, 0.2f, 10.0f);
    this->cubeSceneNode->SetPosition(2.0f, -2.8f, -2.0f);
    this->cubeSceneNode->AttachObject(this->cubeEntity);
    this->vMod = 0.0f;

    this->lightConeMaterial = new Material();
    pass = this->lightConeMaterial->CreatePass();
    pass->SetLightingEnabled(true);
    pass->SetSelfIllumination(0xff00ffff);
    this->lightConeEntity = SceneManager::Instance()->CreateEntity("cone.mesh");
    this->lightConeEntity->GetSubEntity(0)->SetMaterial(this->lightConeMaterial);
    this->lightConeSceneNode = SceneManager::Instance()->GetRootSceneNode()->CreateChildSceneNode();
    this->lightConeSceneNode->SetScale(2.0f, 2.0f,2.0f);
    this->lightConeSceneNode->SetPosition(4.0f, 3.0f, -4.0f);
    this->lightConeSceneNode->Yaw(2.14f);
    this->lightConeSceneNode->Pitch(-1.0f);
    this->lightConeSceneNode->AttachObject(this->lightConeEntity);
    
    this->camera = SceneManager::Instance()->GetCamera("MainCamera");
    this->camera->SetPosition(0.0f, 0.0f, 5.0f);
    
    sceKernelDcacheWritebackAll();
}

//------------------------------------------------------------------------------
/**
*/
void
StateMaterialTest::Exit()
{	
    this->solidMaterial->RemoveAllPasses();
    delete this->solidMaterial;
    this->alphaMaterial->RemoveAllPasses();
    delete this->alphaMaterial;
    
    this->cubeMaterial->RemoveAllPasses();
    delete this->cubeMaterial;
    
    this->lightConeMaterial->RemoveAllPasses();
    delete this->lightConeMaterial;

    SceneManager::Instance()->ClearScene();
    MeshManager::Instance()->RemoveAll();   
    TextureManager::Instance()->RemoveAll();
}

//------------------------------------------------------------------------------
/**
*/
void
StateMaterialTest::Trigger()
{ 
    // camera fps controls
    sceCtrlReadBufferPositive(&this->pad, 1);
    float tx = (this->pad.Lx - 128) / 127.0f;
    if (tx > 0.2f)
    {
        tx -= 0.2f;
    }
    else if (tx < -0.2f)
    {
        tx += 0.2f;
    }
    else
    {
        tx = 0.0f;
    }
    this->camera->Yaw(2.0f * -tx * (GU_PI / 180.0f));
    
    float ty = (this->pad.Ly - 128) / 127.0f;
    if (ty > 0.2f)
    {
        ty -= 0.2f;
    }
    else if (ty < -0.2f)
    {
        ty += 0.2f;
    }
    else
    {
        ty = 0.0f;
    }
    this->camera->Pitch(2.0f * ty * (GU_PI / 180.0f));
    
    if (this->pad.Buttons & PSP_CTRL_TRIANGLE) this->camera->MoveRelative(Vector3(0.0f, 0.0f, -0.1f));
    if (this->pad.Buttons & PSP_CTRL_SQUARE)   this->camera->MoveRelative(Vector3(0.0f, 0.0f, 0.1f));

    const chrissly::core::Vector3& pos = this->gothSceneNode->GetPosition();
    if (this->pad.Buttons & PSP_CTRL_LEFT)  this->gothSceneNode->SetPosition(pos.x - 0.05f, pos.y, pos.z);
    if (this->pad.Buttons & PSP_CTRL_RIGHT) this->gothSceneNode->SetPosition(pos.x + 0.05f, pos.y, pos.z);
    if (this->pad.Buttons & PSP_CTRL_UP)    this->gothSceneNode->SetPosition(pos.x, pos.y, pos.z - 0.05f);
    if (this->pad.Buttons & PSP_CTRL_DOWN)  this->gothSceneNode->SetPosition(pos.x, pos.y, pos.z + 0.05f);
    
    // test uv animation
    this->vMod += 0.003f;
    this->cubeMaterial->GetPass(1)->GetTextureUnitState(0)->SetTextureScroll(0.0f, this->vMod);
    
    // rotate scenenode
    this->gothSceneNode->Yaw(0.01f);

    GraphicsSystem::Instance()->RenderOneFrame();

    if (this->pad.Buttons & PSP_CTRL_CROSS) StateManager::Instance()->ChangeState(StateAnimationTest::Instance());
}

} // namespace application


