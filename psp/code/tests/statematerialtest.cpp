//------------------------------------------------------------------------------
//  statematerialtest.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "statematerialtest.h"
#include "stateanimationtest.h"
#include "statemanager.h"
#include "graphicssystem.h"
#include "scenemanager.h"
#include "chrisslyinput.h"

using namespace chrissly::core;
using namespace chrissly::graphics;
using namespace chrissly::application;

StateMaterialTest* StateMaterialTest::Singleton = NULL;

//------------------------------------------------------------------------------
/**
*/
StateMaterialTest::StateMaterialTest() :
    camera(NULL),
    gothSceneNode(NULL),
    cubeMaterial(NULL),
    lightConeSceneNode(NULL),
    spotLight(NULL),
    vMod(0.0f)
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
    MaterialManager::Instance()->Initialise();

    this->camera = SceneManager::Instance()->GetCamera("MainCamera");
    this->camera->SetPosition(0.0f, 0.0f, 10.0f);
    this->camera->SetOrientation(Quaternion());

    Entity* gothEntity = SceneManager::Instance()->CreateEntity("gothic_woman.mesh");
    gothEntity->SetCastShadows(true);
    this->gothSceneNode = SceneManager::Instance()->GetRootSceneNode()->CreateChildSceneNode();
    this->gothSceneNode->SetScale(3.5f, 3.5f, 3.5f);
    this->gothSceneNode->SetPosition(-2.0f, 0.1f, -1.4f);
    this->gothSceneNode->AttachObject(gothEntity);

    Entity* cubeEntity = SceneManager::Instance()->CreateEntity("cube.mesh");
    cubeEntity->SetReceivesShadows(true);
    SceneNode* cubeSceneNode = SceneManager::Instance()->GetRootSceneNode()->CreateChildSceneNode();
    cubeSceneNode->SetScale(10.0f, 0.2f, 10.0f);
    cubeSceneNode->SetPosition(2.0f, -2.8f, -2.0f);
    cubeSceneNode->AttachObject(cubeEntity);
    this->cubeMaterial = MaterialManager::Instance()->GetByName("cube_material");
    this->vMod = 0.0f;

    Entity* lightConeEntity = SceneManager::Instance()->CreateEntity("cone.mesh");
    this->lightConeSceneNode = SceneManager::Instance()->GetRootSceneNode()->CreateChildSceneNode();
    this->lightConeSceneNode->SetScale(2.0f, 2.0f,2.0f);
    this->lightConeSceneNode->SetPosition(4.0f, 4.0f, -4.0f);
    this->lightConeSceneNode->Yaw(2.14f);
    this->lightConeSceneNode->Pitch(-1.0f);
    this->lightConeSceneNode->AttachObject(lightConeEntity);

    SceneManager::Instance()->SetAmbientLight(0xff222222);
    this->spotLight = SceneManager::Instance()->CreateLight("SpotLight");
    this->spotLight->SetType(Light::LT_SPOTLIGHT);
    this->spotLight->SetDiffuseColour(0xff0077ff);
    this->spotLight->SetSpecularColour(0x00000000);
    this->spotLight->SetAttenuation(100.0f, 1.0f, 0.0f, 0.0f);
    this->spotLight->SetPosition(4.0f, 4.0f, -4.0f);
    this->spotLight->SetDirection(this->lightConeSceneNode->GetOrientation() * Vector3(0.0f, 0.0f, -1.0f));
    this->spotLight->SetSpotlightOuterAngle(0.78f);
    this->spotLight->SetSpotlightFalloff(0.1f);
    this->spotLight->SetCastShadows(true);
    Light* light = SceneManager::Instance()->CreateLight("PointLight");
    light->SetType(Light::LT_POINT);
    light->SetDiffuseColour(0xff777777);
    light->SetSpecularColour(0x00000000);
    light->SetAttenuation(100.0f, 1.0f, 0.0f, 0.0f);
    light->SetPosition(0.0f, -6.0f, -1.4f);
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
    ce_gamepad_state pad;
    ce_input_gamepad_get_state(&pad);

    // camera fps controls
    this->camera->Yaw(2.0f * -pad.left_stick_x * (GU_PI / 180.0f));
    this->camera->Pitch(2.0f * pad.left_stick_y * (GU_PI / 180.0f));
    if (pad.buttons & GAMEPAD_TRIANGLE) this->camera->MoveRelative(Vector3(0.0f, 0.0f, -0.1f));
    if (pad.buttons & GAMEPAD_SQUARE)   this->camera->MoveRelative(Vector3(0.0f, 0.0f, 0.1f));

    // move object
    const chrissly::core::Vector3& pos = this->gothSceneNode->GetPosition();
    if (pad.buttons & GAMEPAD_DPAD_LEFT)  this->gothSceneNode->SetPosition(pos.x - 0.05f, pos.y, pos.z);
    if (pad.buttons & GAMEPAD_DPAD_RIGHT) this->gothSceneNode->SetPosition(pos.x + 0.05f, pos.y, pos.z);
    if (pad.buttons & GAMEPAD_DPAD_UP)    this->gothSceneNode->SetPosition(pos.x, pos.y, pos.z - 0.05f);
    if (pad.buttons & GAMEPAD_DPAD_DOWN)  this->gothSceneNode->SetPosition(pos.x, pos.y, pos.z + 0.05f);

    // rotate light
    if (pad.buttons & GAMEPAD_LEFT_SHOULDER)
    {
        this->lightConeSceneNode->Pitch(0.01f);
        this->spotLight->SetDirection(this->lightConeSceneNode->GetOrientation() * Vector3(0.0f, 0.0f, -1.0f));
    }
    if (pad.buttons & GAMEPAD_RIGHT_SHOULDER)
    {
        this->lightConeSceneNode->Pitch(-0.01f);
        this->spotLight->SetDirection(this->lightConeSceneNode->GetOrientation() * Vector3(0.0f, 0.0f, -1.0f));
    }

    // test uv animation
    this->vMod += 0.003f;
    this->cubeMaterial->GetPass(1U)->GetTextureUnitState(0U)->SetTextureScroll(0.0f, this->vMod);

    // rotate scenenode
    this->gothSceneNode->Yaw(0.01f);

    GraphicsSystem::Instance()->RenderOneFrame();

    if (pad.buttons & GAMEPAD_CROSS) StateManager::Instance()->ChangeState(StateAnimationTest::Instance());
}