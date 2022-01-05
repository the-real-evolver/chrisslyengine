//------------------------------------------------------------------------------
//  main.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "debug.h"
#include "graphicssystem.h"
#include "audiosystem.h"
#include "channel.h"
#include <windows.h>

using namespace chrissly::core;
using namespace chrissly::graphics;
using namespace chrissly::audio;

static const unsigned int DelayInSamples = 35280U;
static const float Decay = 0.6f;
static int delayBuffer[65535U];
static unsigned int delaySamplePosition = 0U;

//------------------------------------------------------------------------------
/**
*/
Result
DspCallback(int numChannels, int bits, unsigned int numSamples, const void* const inBuffer, void* const outBuffer, void* const userData)
{
    CE_ASSERT(2 == numChannels && 16 == bits, "DspCallback(): only 16 Bit stereo sounds are supported\n");
    CE_UNREFERENCED_PARAMETER(numChannels);
    CE_UNREFERENCED_PARAMETER(bits);
    CE_UNREFERENCED_PARAMETER(userData);

    const short* input = (const short*)inBuffer;
    short* output = (short*)outBuffer;
    const int* buffer = (const int*)inBuffer;
    unsigned int i;
    for (i = 0U; i < numSamples; ++i)
    {
        /* 1. mix delaybuffer with input */
        if (delaySamplePosition >= DelayInSamples) {delaySamplePosition = 0U;}
        const short* delaySample = (short*)&delayBuffer[delaySamplePosition];
        *output++ = *input++ + (short)((float)(*delaySample++) * Decay);
        *output++ = *input++ + (short)((float)(*delaySample) * Decay);

        /* 2. fill delaybuffer with input */
        delayBuffer[delaySamplePosition++] = buffer[i];
    }

    return OK;
}

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
    camera->SetPosition(-0.5f, -0.25f, 1.0f);
    camera->SetOrientation(Quaternion());
    window->AddViewport(camera, 0, 0, 1280, 720);

    MaterialManager::Instance()->Initialise();
    Entity* entity = SceneManager::Instance()->CreateEntity("gothic_woman.mesh");
    SceneNode* sceneNode = SceneManager::Instance()->GetRootSceneNode()->CreateChildSceneNode();
    sceneNode->AttachObject(entity);
    sceneNode->SetPosition(-0.5f, 0.0f, 0.0f);
    sceneNode->SetScale(1.0f, 1.0f, 1.0f);

    Light* light = SceneManager::Instance()->CreateLight("WhitePointLight");
    light->SetType(Light::LT_POINT);
    light->SetDiffuseColour(0xffffffff);
    light->SetSpecularColour(0xffffffff);
    light->SetPosition(-0.5f, 1.5f, 1.5f);
    light = SceneManager::Instance()->CreateLight("RedPointLight");
    light->SetType(Light::LT_POINT);
    light->SetDiffuseColour(0xff0000ff);
    light->SetPosition(-2.0f, 0.0f, 0.0f);
    light = SceneManager::Instance()->CreateLight("MagentaPointLight");
    light->SetType(Light::LT_POINT);
    light->SetDiffuseColour(0xffff00ff);
    light->SetPosition(1.0f, 0.0f, 0.0f);

    AudioSystem* audioSystem = new AudioSystem();
    audioSystem->Initialise();
    Sound* sound;
    audioSystem->CreateSound("intro.ogg", MODE_CREATESAMPLE | MODE_LOOP_NORMAL, &sound);
    Channel* channel;
    audioSystem->PlaySound(Channel::CHANNEL_FREE, sound, false, &channel);

    memset(delayBuffer, 0, sizeof(delayBuffer));
    DspDescription dspDesc = {DspCallback};
    DSP* dsp;
    audioSystem->CreateDSP(&dspDesc, &dsp);
    channel->AddDSP(0U, dsp);
    bool bypassDsp = false;

    MSG msg = {0};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            if (WM_KEYDOWN == msg.message)
            {
                CE_LOG("-> Key Pressed: '%c'\n", (char)msg.wParam);
                if (VK_SPACE == msg.wParam)
                {
                    bypassDsp = !bypassDsp;
                    dsp->SetBypass(bypassDsp);
                }
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        sceneNode->Yaw(0.01f);
        graphicsSystem->RenderOneFrame();
        audioSystem->Update();
    }

    delete graphicsSystem;

    delete audioSystem;

    return (int)msg.wParam;
}
