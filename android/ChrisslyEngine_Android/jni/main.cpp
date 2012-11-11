//------------------------------------------------------------------------------
//  main.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include <android/sensor.h>
#include <android_native_app_glue.h>
#include "graphics/graphicssystem.h"
#include "graphics/scenemanager.h"
#include "graphics/renderwindow.h"
#include "graphics/camera.h"
#include "graphics/animationstate.h"
#include "core/fswrapper.h"

using namespace chrissly::graphics;
using namespace chrissly::core;
using namespace chrissly;

bool initialized = false;
GraphicsSystem* graphicsSystem;
SceneNode* sceneNode;
Entity* entity;
Material* material;
Texture* tex;
Mesh* mesh;
Camera* camera;
int32_t lastX, lastY, distance;
AnimationState* animState;
GpuProgram* gpuProgram;

const char* MorphAnimVertexShader =
    "attribute vec2 texCoordIn;\n"
    "varying vec2 texCoordOut;\n"
    "attribute vec4 position;\n"
	"attribute vec4 positionMorphTarget;\n"
    "uniform mat4 worldMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "uniform mat4 projectionMatrix;\n"
    "uniform mat4 worldViewProjMatrix;\n"
	"uniform float morphWeight;\n"
    "void main()\n"
    "{\n"
	"    gl_Position = worldViewProjMatrix * (position + (positionMorphTarget - position) * morphWeight);\n"
    "    texCoordOut = texCoordIn;\n"
    "}\n";

const char* FragmentShader =
    "varying lowp vec2 texCoordOut;\n"
    "uniform sampler2D texture;\n"
    "precision mediump float;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0) * texture2D(texture, texCoordOut);\n"
    "}\n";

//------------------------------------------------------------------------------
/**
*/
void
Enter(struct android_app* state)
{
    graphicsSystem = new GraphicsSystem();
    RenderWindow* window = graphicsSystem->Initialise(state->window);
    camera = SceneManager::Instance()->CreateCamera("MainCamera");
    camera->SetAspectRatio((float)window->GetWidth() / (float)window->GetHeight());
    camera->SetPosition(0.0f, 0.0f, 0.9f);
    window->AddViewport(camera, 0, 0, window->GetWidth(), window->GetHeight());

    material = new Material();
    Pass* pass = material->CreatePass();
    pass->SetCullingMode(CULL_NONE);
    pass->SetSceneBlendingEnabled(false);
    pass->SetSceneBlending(SBF_SOURCE_COLOUR, SBF_DEST_COLOUR);
    gpuProgram = CE_NEW GpuProgram(MorphAnimVertexShader, FragmentShader);
    pass->SetGpuProgram(gpuProgram);
    tex = TextureManager::Instance()->Load("cerberus_etc1.tex");
    TextureUnitState* tus = pass->CreateTextureUnitState();
    tus->SetTexture(tex);

    entity = SceneManager::Instance()->CreateEntity("cerberus_walk.mesh");
    entity->GetSubEntity(0)->SetMaterial(material);
    animState = entity->GetAnimationState("default");
    animState->SetEnabled(true);
    animState->SetLoop(true);

    sceneNode = SceneManager::Instance()->GetRootSceneNode()->CreateChildSceneNode();
    sceneNode->AttachObject(entity);
    sceneNode->SetPosition(0.0f, 0.0f, 0.0f);

    initialized = true;
}

//------------------------------------------------------------------------------
/**
*/
void
Exit()
{
    initialized = false;

    CE_DELETE gpuProgram;
    material->RemoveAllPasses();
    delete material;

    delete graphicsSystem;
}

//------------------------------------------------------------------------------
/**
*/
void
Trigger()
{
    if (initialized)
    {
    	animState->AddTime(0.016f);
        sceneNode->Yaw((float)distance * 0.02f);
        graphicsSystem->RenderOneFrame();
    }
}

//------------------------------------------------------------------------------
/**
    process the next input event
*/
static int32_t
HandleInputEvents(struct android_app* app, AInputEvent* event)
{
    if (AINPUT_EVENT_TYPE_MOTION == AInputEvent_getType(event))
    {
        int32_t x = AMotionEvent_getX(event, 0);
        distance = x - lastX;
        lastX = x;
        lastY = AMotionEvent_getY(event, 0);
        return 1;
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    process the next main command
*/
static void
HandleCommands(struct android_app* app, int32_t cmd)
{
    switch (cmd)
    {
        case APP_CMD_SAVE_STATE:
            // the system has asked us to save our current state
            break;
        case APP_CMD_INIT_WINDOW:
            // the window is being shown, get it ready
            if (app->window != NULL)
            {
                Enter(app);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // the window is being hidden or closed, clean it up
            Exit();
            break;
        case APP_CMD_CONFIG_CHANGED:
            if (initialized)
            {
                Exit();
            }
            Enter(app);
            break;
        case APP_CMD_GAINED_FOCUS:
            // app gained focus
            break;
        case APP_CMD_LOST_FOCUS:
            break;
    }
}

//------------------------------------------------------------------------------
/**
    this is the main entry point of a native application that is using
    android_native_app_glue.  It runs in its own thread, with its own
    event loop for receiving input events and doing other things
*/
void
android_main(struct android_app* state)
{
    // make sure glue isn't stripped
    app_dummy();

    // set callbacks
    state->onAppCmd = HandleCommands;
    state->onInputEvent = HandleInputEvents;

    // initialize filesystem
    FSWrapper::_Initialise(state->activity->assetManager);

    while (true)
    {
        // read all pending events
        int ident;
        int events;
        struct android_poll_source* source;

        // loop until all events are read, then continue
        while ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0)
        {
            // process this event
            if (source != NULL)
            {
                source->process(state, source);
            }

            // check if we are exiting
            if (state->destroyRequested != 0)
            {
                return;
            }
        }

        // draw the next frame
        Trigger();
    }
}
