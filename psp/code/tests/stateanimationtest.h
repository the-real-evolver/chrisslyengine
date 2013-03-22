#ifndef STATEANIMATIONTEST_H_
#define STATEANIMATIONTEST_H_
//------------------------------------------------------------------------------
/**
    @class StateAnimationTest

    13.03.2011 -> Creation

    (C) 2011 Christian Bleicher
*/
#include "state.h"
#include "texture.h"
#include "material.h"
#include "mesh.h"
#include "entity.h"
#include "camera.h"
#include "light.h"
#include <pspctrl.h>

//------------------------------------------------------------------------------

class StateAnimationTest : public chrissly::application::State
{
public:
    /// get pointer to the singleton
    static StateAnimationTest* Instance()
    {
        return Singleton;
    };

    /// default constructor
    StateAnimationTest();
    /// destructor
    virtual	~StateAnimationTest();
    /// called when entering the state
    void Enter();
    /// called when leaving the state
    void Exit();
    /// trigger the state
    void Trigger();

private:
    /// copy constructor
    StateAnimationTest(const StateAnimationTest&cc) {};

    static StateAnimationTest* Singleton;

    chrissly::graphics::SceneNode* sceneNode;
    chrissly::graphics::Entity* entity;
    chrissly::graphics::AnimationState* animState;

    chrissly::graphics::Material* material;
    chrissly::graphics::Pass* pass0;
    chrissly::graphics::Texture* tex0;

    chrissly::graphics::Camera* camera;

    bool loop;
    bool pause;

    SceCtrlData pad;
};

//------------------------------------------------------------------------------
#endif

