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

using namespace chrissly::graphics;
using namespace chrissly::core;
using namespace chrissly::application;

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
    Material* solidMaterial = MaterialManager::Instance()->Create("solidMaterial");
    Texture* tex = TextureManager::Instance()->Load("gothic_solid.tex");
    Pass* pass = solidMaterial->CreatePass();
    pass->SetFog(FOG_LINEAR, 0xff0080ff, 0.0f, 50.0f);
    pass->SetLightingEnabled(true);
    pass->SetVertexColourTracking(TVC_DIFFUSE | TVC_AMBIENT | TVC_SPECULAR);
    pass->SetSpecular(12.0f);
    TextureUnitState* tus = pass->CreateTextureUnitState();
    tus->SetTexture(tex);
    tus->SetTextureBlendOperation(LBT_COLOUR, LBO_MODULATE);

    // transparent material
    Material* alphaMaterial = MaterialManager::Instance()->Create("alphaMaterial");
    tex = TextureManager::Instance()->Load("gothic_alpha.tex");
    pass = alphaMaterial->CreatePass();
    pass->SetSceneBlendingEnabled(true);
    pass->SetSceneBlending(SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
    pass->SetCullingMode(CULL_NONE);
    pass->SetFog(FOG_LINEAR, 0xff0080ff, 0.0f, 50.0f);
    pass->SetLightingEnabled(true);
    pass->SetVertexColourTracking(TVC_DIFFUSE | TVC_AMBIENT | TVC_SPECULAR);
    pass->SetSpecular(12.0f);
    tus = pass->CreateTextureUnitState();
    tus->SetTexture(tex);
    tus->SetTextureBlendOperation(LBT_ALPHA, LBO_MODULATE);

    // create scenenode, entity and set materials
    Entity* gothEntity = SceneManager::Instance()->CreateEntity("gothic_woman.mesh");
    gothEntity->SetCastShadows(true);
    gothEntity->GetSubEntity(1)->SetMaterial(solidMaterial);
    gothEntity->GetSubEntity(0)->SetMaterial(alphaMaterial);
    gothEntity->GetSubEntity(2)->SetMaterial(alphaMaterial);
    this->gothSceneNode = SceneManager::Instance()->GetRootSceneNode()->CreateChildSceneNode();
    this->gothSceneNode->SetScale(3.5f, 3.5f, 3.5f);
    this->gothSceneNode->SetPosition(-2.0f, 0.1f, -1.4f);
    this->gothSceneNode->AttachObject(gothEntity);

    this->cubeMaterial = MaterialManager::Instance()->Create("cubeMaterial");
    tex = TextureManager::Instance()->Load("floor.tex");
    pass = this->cubeMaterial->CreatePass();
    tus = pass->CreateTextureUnitState();
    tus->SetTextureBlendOperation(LBT_COLOUR, LBO_REPLACE);
    tus->SetTexture(tex);
    tex = TextureManager::Instance()->Load("water.tex");
    pass = this->cubeMaterial->CreatePass();
    pass->SetSceneBlendingEnabled(true);
    pass->SetSceneBlending(SBF_SOURCE_COLOUR, SBF_DEST_COLOUR);
    tus = pass->CreateTextureUnitState();
    tus->SetTexture(tex);
    tus->SetTextureBlendOperation(LBT_COLOUR, LBO_REPLACE);

    Entity* cubeEntity = SceneManager::Instance()->CreateEntity("cube.mesh");
    cubeEntity->GetSubEntity(0)->SetMaterial(this->cubeMaterial);
    cubeEntity->SetReceivesShadows(true);
    SceneNode* cubeSceneNode = SceneManager::Instance()->GetRootSceneNode()->CreateChildSceneNode();
    cubeSceneNode->SetScale(10.0f, 0.2f, 10.0f);
    cubeSceneNode->SetPosition(2.0f, -2.8f, -2.0f);
    cubeSceneNode->AttachObject(cubeEntity);
    this->vMod = 0.0f;

    Material* lightConeMaterial = MaterialManager::Instance()->Create("lightConeMaterial");
    pass = lightConeMaterial->CreatePass();
    Entity* lightConeEntity = SceneManager::Instance()->CreateEntity("cone.mesh");
    lightConeEntity->GetSubEntity(0)->SetMaterial(lightConeMaterial);
    SceneNode* lightConeSceneNode = SceneManager::Instance()->GetRootSceneNode()->CreateChildSceneNode();
    lightConeSceneNode->SetScale(2.0f, 2.0f,2.0f);
    lightConeSceneNode->SetPosition(4.0f, 3.0f, -4.0f);
    lightConeSceneNode->Yaw(2.14f);
    lightConeSceneNode->Pitch(-1.0f);
    lightConeSceneNode->AttachObject(lightConeEntity);

    this->camera = SceneManager::Instance()->GetCamera("MainCamera");
    this->camera->SetPosition(0.0f, 0.0f, 5.0f);
    this->camera->SetOrientation(Quaternion());

    SceneManager::Instance()->SetAmbientLight(0xff222222);
    Light* light = SceneManager::Instance()->CreateLight("SpotLight");
    light->SetType(Light::LT_SPOTLIGHT);
    light->SetDiffuseColour(0xff0077ff);
    light->SetSpecularColour(0x00000000);
    light->SetAttenuation(1.0f, 0.0f, 0.0f);
    light->SetPosition(6.0f, 6.0f, -1.4f);
    light->SetDirection(-1.0f, -1.0f, 0.0f);
    light->SetSpotlightFalloff(5.0f);
    light->SetSpotlightOuterAngle(0.6f);
    light = SceneManager::Instance()->CreateLight("PointLight");
    light->SetType(Light::LT_POINT);
    light->SetDiffuseColour(0xff777777);
    light->SetSpecularColour(0x00000000);
    light->SetAttenuation(1.0f, 0.0f, 0.0f);
    light->SetPosition(0.0f, -6.0f, -1.4f);

    sceKernelDcacheWritebackAll();
}

//------------------------------------------------------------------------------
/**
*/
void
StateMaterialTest::Exit()
{
    SceneManager::Instance()->ClearScene();
    MeshManager::Instance()->RemoveAll();
    TextureManager::Instance()->RemoveAll();
    MaterialManager::Instance()->RemoveAll();
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



