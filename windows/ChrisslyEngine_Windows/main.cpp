//------------------------------------------------------------------------------
//  main.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "debug.h"
#include "graphicssystem.h"
#include <windows.h>

using namespace chrissly::core;
using namespace chrissly::graphics;

//------------------------------------------------------------------------------
/**
*/
int WINAPI
WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prevInstance, _In_ LPSTR cmdLine, _In_ int showCmd)
{
    CE_UNREFERENCED_PARAMETER(cmdLine);
    CE_UNREFERENCED_PARAMETER(showCmd);
    CE_UNREFERENCED_PARAMETER(prevInstance);

    GraphicsSystem* graphicsSystem = new GraphicsSystem();
    RenderWindow* window = graphicsSystem->Initialise((void*)instance);
    Camera* camera = SceneManager::Instance()->CreateCamera("MainCamera");
    window->AddViewport(camera, 0, 0, 960, 544);

    MaterialManager::Instance()->Initialise();
    Entity* entity = SceneManager::Instance()->CreateEntity("gothic_woman.mesh");
    SceneNode* sceneNode = SceneManager::Instance()->GetRootSceneNode()->CreateChildSceneNode();
    sceneNode->AttachObject(entity);
    sceneNode->SetPosition(-0.5f, 0.0f, 0.0f);
    sceneNode->SetScale(1.0f, 1.0f, 1.0f);
    camera->SetPosition(-0.5f, -0.25f, 1.0f);
    camera->SetOrientation(Quaternion());

    Light* light = SceneManager::Instance()->CreateLight("WhitePointLight");
    light->SetType(Light::LT_POINT);
    light->SetDiffuseColour(0xffffffff);
    light->SetPosition(-0.5f, 1.5f, 1.5f);
    light = SceneManager::Instance()->CreateLight("RedPointLight");
    light->SetType(Light::LT_POINT);
    light->SetDiffuseColour(0xff0000ff);
    light->SetPosition(-2.0f, 0.0f, 0.0f);
    light = SceneManager::Instance()->CreateLight("MagentaPointLight");
    light->SetType(Light::LT_POINT);
    light->SetDiffuseColour(0xffff00ff);
    light->SetPosition(1.0f, 0.0f, 0.0f);

    MSG msg = {0};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            if (WM_KEYDOWN == msg.message)
            {
                CE_LOG("-> Key Pressed: '%c'\n", (char)msg.wParam);
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        sceneNode->Yaw(0.01f);
        GraphicsSystem::Instance()->RenderOneFrame();
    }

    delete graphicsSystem;

    return (int)msg.wParam;
}
