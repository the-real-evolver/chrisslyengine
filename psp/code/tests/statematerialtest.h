#ifndef STATEMATERIALTEST_H_
#define STATEMATERIALTEST_H_
//------------------------------------------------------------------------------
/**
    @class StateMaterialTest

    08.05.2011 -> Creation

    (C) 2011 Christian Bleicher
*/
#include "state.h"
#include "material.h"
#include "scenenode.h"
#include "camera.h"
#include <pspctrl.h>

//------------------------------------------------------------------------------
class StateMaterialTest : public chrissly::application::State
{
public:
    /// get pointer to the singleton
    static StateMaterialTest* Instance()
    {
        return Singleton;
    };

    /// default constructor
    StateMaterialTest();
    /// destructor
    virtual	~StateMaterialTest();
    /// called when entering the state
    void Enter();
    /// called when leaving the state
    void Exit();
    /// trigger the state
    void Trigger();

private:
    /// copy constructor
    StateMaterialTest(const StateMaterialTest&cc) {};

    static StateMaterialTest* Singleton;

    chrissly::graphics::SceneNode* gothSceneNode;

    chrissly::graphics::Material* cubeMaterial;
    float vMod;

    chrissly::graphics::Camera* camera;
    SceCtrlData pad;
};

//------------------------------------------------------------------------------
#endif

