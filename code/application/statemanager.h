#ifndef STATEMANAGER_H_
#define STATEMANAGER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::application::StateManager

    (C) 2010 Christian Bleicher
*/
#include "state.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace application
{

class StateManager
{
public:
    /// get pointer to the singleton
    static StateManager* Instance()
    {
        return Singleton;
    };

    /// default constructor
    StateManager();
    /// destructor
    ~StateManager();
    /// updates the statemanager
    void Trigger();
    /// changes the state
    void ChangeState(State* state);
    /// shutdown the statemanger
    void Exit();
    /// returns if the statemanger is running
    bool IsRunning();

private:
    /// copy constructor
    StateManager(const StateManager&cc) {};

    static StateManager* Singleton;
    State* currentState;
    bool isRunning;
};

} // namespace application
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
