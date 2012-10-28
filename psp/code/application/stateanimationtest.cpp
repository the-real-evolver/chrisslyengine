//------------------------------------------------------------------------------
//  stateanimationtest.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "stateanimationtest.h"
#include "statematerialtest.h"
#include "statemanager.h"
#include "graphicssystem.h"
#include "debug.h"

namespace application
{

using namespace chrissly;
using namespace chrissly::graphics;

StateAnimationTest* StateAnimationTest::Singleton = NULL;

//------------------------------------------------------------------------------
/**
*/
StateAnimationTest::StateAnimationTest() :
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
    sceCtrlSetSamplingCycle(100);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    
    this->material = new Material();
    this->pass0 = this->material->CreatePass();
    this->tex0 = TextureManager::Instance()->Load("cerberus.tex");
    TextureUnitState* tus = this->pass0->CreateTextureUnitState();
    tus->SetTexture(this->tex0);

    this->entity = SceneManager::Instance()->CreateEntity("cerberus_walk.mesh");
    this->entity->GetSubEntity(0)->SetMaterial(this->material);
    this->animState = this->entity->GetAnimationState("default");
    CE_ASSERT(this->animState != NULL, "StateAnimationTest::Enter(): this->animState invalid");
    this->animState->SetEnabled(true);

    this->sceneNode = SceneManager::Instance()->GetRootSceneNode()->CreateChildSceneNode();
    this->sceneNode->SetScale(3.5f, 3.5f, 3.5f);
    this->sceneNode->SetPosition(0.0f, 0.1f, -1.4f);
    this->sceneNode->AttachObject(this->entity);

    this->camera = SceneManager::Instance()->GetCamera("MainCamera");
    this->camera->SetPosition(0.0f, 0.0f, 2.0f);
    this->camera->SetOrientation(core::Quaternion());
}

//------------------------------------------------------------------------------
/**
*/
void
StateAnimationTest::Exit()
{
    this->material->RemoveAllPasses();
    delete this->material;

    SceneManager::Instance()->ClearScene();
    MeshManager::Instance()->RemoveAll();
    TextureManager::Instance()->RemoveAll();
}

//------------------------------------------------------------------------------
/**
*/
void
StateAnimationTest::Trigger()
{
    // controls
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
    if (this->pad.Buttons & PSP_CTRL_SQUARE) this->pause = !this->pause;
    if (this->pad.Buttons & PSP_CTRL_TRIANGLE) this->loop = !this->loop;

    // update animation
    this->animState->SetLoop(this->loop);
    if (!this->pause) this->animState->AddTime(0.016f);

    this->sceneNode->Yaw(tx * 0.05f);
    GraphicsSystem::Instance()->RenderOneFrame();

    if (this->pad.Buttons & PSP_CTRL_CROSS)
    {
        StateManager::Instance()->ChangeState(StateMaterialTest::Instance());
    }
}

} // namespace application


