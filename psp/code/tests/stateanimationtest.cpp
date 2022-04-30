//------------------------------------------------------------------------------
//  stateanimationtest.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "stateanimationtest.h"
#include "statematerialtest.h"
#include "statemanager.h"
#include "graphicssystem.h"
#include "chrisslyinput.h"

using namespace chrissly::core;
using namespace chrissly::graphics;
using namespace chrissly::application;

StateAnimationTest* StateAnimationTest::Singleton = NULL;

//------------------------------------------------------------------------------
/**
*/
StateAnimationTest::StateAnimationTest() :
    sceneNode(NULL),
    animState(NULL),
    loop(true),
    pause(false)
{
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
StateAnimationTest::~StateAnimationTest()
{
    Singleton = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
StateAnimationTest::Enter()
{
    MaterialManager::Instance()->Initialise();

    Material* material = MaterialManager::Instance()->GetByName("cerberus_material_psp");
    Entity* entity = SceneManager::Instance()->CreateEntity("cerberus_walk.mesh");
    entity->GetSubEntity(0U)->SetMaterial(material);

    this->animState = entity->GetAnimationState("default");
    this->animState->SetEnabled(true);

    this->sceneNode = SceneManager::Instance()->GetRootSceneNode()->CreateChildSceneNode();
    this->sceneNode->SetScale(4.5f, 4.5f, 4.5f);
    this->sceneNode->SetPosition(0.0f, 0.1f, -1.4f);
    this->sceneNode->AttachObject(entity);

    Camera* camera = SceneManager::Instance()->GetCamera("MainCamera");
    camera->SetPosition(0.0f, 0.2f, 2.0f);
    camera->SetOrientation(Quaternion());
}

//------------------------------------------------------------------------------
/**
*/
void
StateAnimationTest::Exit()
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
StateAnimationTest::Trigger()
{
    // controls
	ce_gamepad_state pad;
    ce_input_gamepad_get_state(&pad);
    if (pad.buttons & GAMEPAD_SQUARE) this->pause = !this->pause;
    if (pad.buttons & GAMEPAD_TRIANGLE) this->loop = !this->loop;

    // update animation
    this->animState->SetLoop(this->loop);
    if (!this->pause) this->animState->AddTime(0.016f);

    this->sceneNode->Yaw(pad.leftStickX * 0.05f);

    GraphicsSystem::Instance()->RenderOneFrame();

    if (pad.buttons & GAMEPAD_CROSS) StateManager::Instance()->ChangeState(StateMaterialTest::Instance());
}