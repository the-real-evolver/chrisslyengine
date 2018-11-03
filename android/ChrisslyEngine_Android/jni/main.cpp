//------------------------------------------------------------------------------
//  main.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include <android/sensor.h>
#include <android_native_app_glue.h>
#include "fswrapper.h"
#include "graphicssystem.h"
#include "audiosystem.h"
#include "channel.h"

using namespace chrissly;
using namespace chrissly::core;
using namespace chrissly::graphics;
using namespace chrissly::audio;

bool initialised = false;
GraphicsSystem* graphicsSystem;
SceneNode* sceneNode;
int32_t lastX, distance;
AnimationState* animState;
GpuProgram* gpuProgram;
GpuProgramParameters* params;
Matrix4 mat;

static const char* const MorphAnimVertexShader =
    "#version 100\n"
    "attribute vec2 texCoordIn;\n"
    "attribute vec3 normal;\n"
    "attribute vec4 position;\n"
    "attribute vec4 positionMorphTarget;\n"
    "uniform mat4 worldMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "uniform mat4 projectionMatrix;\n"
    "uniform mat4 worldViewProjMatrix;\n"
    "uniform float morphWeight;\n"
    "varying vec2 texCoordOut;\n"
    "varying vec3 fragmentNormal;\n"
    "varying vec3 cameraVector;\n"
    "varying vec3 lightVector;\n"
    "void main()\n"
    "{\n"
    "    vec4 pos = (position + (positionMorphTarget - position) * morphWeight);\n"
    "    gl_Position = worldViewProjMatrix * pos;\n"
    "    texCoordOut = texCoordIn;\n"
    "    fragmentNormal = mat3(worldMatrix) * normal;\n"
    "    cameraVector = vec3(0.0, 0.0, 0.9) - (worldMatrix * pos).xyz;\n"
    "    lightVector = vec3(1.0, 0.0, 0.0) - (worldMatrix * pos).xyz;\n"
    "}\n";

static const char* const FragmentShader =
    "#version 100\n"
    "precision mediump float;\n"
    "const float MaxDist = 2.5;\n"
    "const float MaxDistSquared = MaxDist * MaxDist;\n"
    "const vec3 lightColor = vec3(0.6, 0.6, 0.6);\n"
    "const float materialPower = 32.0;\n"
    "varying vec2 texCoordOut;\n"
    "varying vec3 fragmentNormal;\n"
    "varying vec3 cameraVector;\n"
    "varying vec3 lightVector;\n"
    "uniform sampler2D texture;\n"
    "uniform mat4 specularColor;\n"
    "void main()\n"
    "{\n"
    "    vec3 normal = normalize(fragmentNormal);\n"
    "    float attenuation = 1.0 - min(dot(lightVector, lightVector), MaxDistSquared) / MaxDistSquared;\n"
    "    vec3 lightDir = normalize(lightVector);\n"
    "    vec3 diffuse = lightColor * clamp(dot(normal, lightDir), 0.0, 1.0) * attenuation;\n"
    "    vec3 halfAngle = normalize(normalize(cameraVector) + lightDir);\n"
    "    float specularDot = dot(normal, halfAngle);\n"
    "    vec3 specular = vec3(specularColor[0][0], specularColor[0][1], specularColor[0][2]) * exp(-2.0 * materialPower * pow(acos(specularDot), 2.0));\n"
    "    gl_FragColor = vec4(clamp((diffuse + texture2D(texture, texCoordOut).rgb) + specular, 0.0, 1.0), 1.0);\n"
    "}\n";

AudioSystem* audioSystem;
Channel* channel;

//------------------------------------------------------------------------------
/**
*/
static void
Enter(struct android_app* state)
{
    graphicsSystem = new GraphicsSystem();
    RenderWindow* window = graphicsSystem->Initialise(state->window);
    Camera* camera = SceneManager::Instance()->CreateCamera("MainCamera");
    camera->SetAspectRatio((float)window->GetWidth() / (float)window->GetHeight());
    camera->SetPosition(0.0f, 0.0f, 0.9f);
    window->AddViewport(camera, 0, 0, window->GetWidth(), window->GetHeight());

    MaterialManager::Instance()->Initialise();
    Material* material = MaterialManager::Instance()->CreateOrRetrieve("cerberus_material_gles");
    gpuProgram = new GpuProgram(MorphAnimVertexShader, FragmentShader);
    material->GetPass(0U)->SetGpuProgram(gpuProgram);

    Entity* entity = SceneManager::Instance()->CreateEntity("cerberus_walk.mesh");
    entity->GetSubEntity(0U)->SetMaterial(material);
    animState = entity->GetAnimationState("default");
    animState->SetEnabled(true);
    animState->SetLoop(true);

    sceneNode = SceneManager::Instance()->GetRootSceneNode()->CreateChildSceneNode();
    sceneNode->AttachObject(entity);
    sceneNode->SetPosition(0.0f, 0.0f, 0.0f);

    params = gpuProgram->GetDefaultParameters();
    mat[0U][0U] = 0.9f; mat[0U][1U] = 0.9f; mat[0U][2U] = 0.9f;
    params->SetNamedConstant("specularColor", mat);

    audioSystem = new AudioSystem();
    audioSystem->Initialise();
    Sound* sound;
    audioSystem->CreateSound("intro.ogg", MODE_CREATESAMPLE | MODE_LOOP_NORMAL, &sound);
    audioSystem->PlaySound(Channel::CHANNEL_FREE, sound, false, &channel);

    initialised = true;
}

//------------------------------------------------------------------------------
/**
*/
static void
Exit()
{
    initialised = false;

    delete gpuProgram;

    delete graphicsSystem;

    delete audioSystem;
}

//------------------------------------------------------------------------------
/**
*/
static void
Trigger()
{
    if (initialised)
    {
        animState->AddTime(0.016f);
        sceneNode->Yaw((float)distance * 0.02f);
        float val = (float)lastX * 0.003f;
        mat[0U][0U] = val; mat[0U][1U] = val; mat[0U][2U] = val;
        params->SetNamedConstant("specularColor", mat);
        graphicsSystem->RenderOneFrame();

        float pan = (((float)lastX / 240.0f) - 0.5f) * 2.0f;
        channel->SetPan(pan);
        audioSystem->Update();
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
        case APP_CMD_DESTROY:
            if (initialised)
            {
                Exit();
            }
            break;
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
            if (initialised)
            {
                Exit();
            }
            break;
        case APP_CMD_CONFIG_CHANGED:
            if (initialised)
            {
                Exit();
            }
            Enter(app);
            break;
        case APP_CMD_GAINED_FOCUS:
            break;
        case APP_CMD_LOST_FOCUS:
            break;
    }
}

//------------------------------------------------------------------------------
/**
    this is the main entry point of a native application that is using
    android_native_app_glue. It runs in its own thread, with its own
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

    // initialise filesystem
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
