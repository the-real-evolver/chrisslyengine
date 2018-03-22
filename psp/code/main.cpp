//------------------------------------------------------------------------------
//  main.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include <pspkernel.h>
#include "graphicssystem.h"
#include "scenemanager.h"
#include "renderwindow.h"
#include "camera.h"
#include "statemanager.h"
#include "stateanimationtest.h"
#include "statematerialtest.h"
#include "debug.h"

PSP_MODULE_INFO("Chrissly Engine PSP", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

using namespace chrissly::graphics;
using namespace chrissly::application;

bool volatile exitRequest = false;

//------------------------------------------------------------------------------
/**
*/
int
ExitCallback(int arg1, int arg2, void* common)
{
    exitRequest = true;
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
main(int argc, char* argv[])
{
    int cbid = sceKernelCreateCallback("ExitCallback", ExitCallback, NULL);
    sceKernelRegisterExitCallback(cbid);

    // create and initialise graphicssystem
    GraphicsSystem* gs = new GraphicsSystem();
    RenderWindow* window = gs->Initialise();
    // add a camera
    Camera* camera = SceneManager::Instance()->CreateCamera("MainCamera");
    // add viewport
    window->AddViewport(camera, 0, 0, 480, 272);
    // enable shadows
    SceneManager::Instance()->SetShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE);

    // create animation teststate
    StateAnimationTest* sat = new StateAnimationTest();
    // create material teststate
    StateMaterialTest* smt = new StateMaterialTest();
    // initialise statemanager
    StateManager* sm = new StateManager();
    sm->ChangeState(sat);

    // initialise input
    sceCtrlSetSamplingCycle(100);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    sceKernelDcacheWritebackAll();

    while (sm->IsRunning() && !exitRequest)
    {
        sm->Trigger();
    }

    sm->Exit();

    delete smt;
    delete sat;
    delete sm;
    delete gs;

    sceKernelExitGame();

    return 0;
}
