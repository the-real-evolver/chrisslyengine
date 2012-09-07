#ifndef STATEMANAGER_H_
#define STATEMANAGER_H_
//------------------------------------------------------------------------------
/**
    @class application::StateManager

    11.06.2007 -> Creation

    (C) 2010 Christian Bleicher
*/
#include "state.h"

//------------------------------------------------------------------------------
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
    /// initialise the statemanager
    void Initialise();
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
//------------------------------------------------------------------------------
#endif
