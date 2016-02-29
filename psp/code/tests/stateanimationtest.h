#ifndef STATEANIMATIONTEST_H_
#define STATEANIMATIONTEST_H_
//------------------------------------------------------------------------------
/**
    @class StateAnimationTest

    (C) 2011 Christian Bleicher
*/
#include "state.h"
#include "scenenode.h"
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
    virtual ~StateAnimationTest();
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
    chrissly::graphics::AnimationState* animState;

    bool loop;
    bool pause;

    SceCtrlData pad;
};

//------------------------------------------------------------------------------
#endif