//------------------------------------------------------------------------------
//  statemanager.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "statemanager.h"
#include "stateanimationtest.h"
#include "statematerialtest.h"

namespace application
{

StateManager* StateManager::Singleton = NULL;

//------------------------------------------------------------------------------
/**
*/
StateManager::StateManager()
{
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
StateManager::~StateManager()
{
    Singleton = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
StateManager::Initialise()
{
    this->currentState = NULL;
    this->isRunning = true;
    this->ChangeState(StateMaterialTest::Instance());
}

//------------------------------------------------------------------------------
/**
*/
void
StateManager::Trigger()
{
    this->currentState->Trigger();
}

//------------------------------------------------------------------------------
/**
*/
void
StateManager::ChangeState(State* state)
{
    if (NULL != currentState)
    {
        this->currentState->Exit();
    }
    
    if (state != this->currentState)
    {
        this->currentState = state;
        state->Enter();
    }	
}

//------------------------------------------------------------------------------
/**
*/
void
StateManager::Exit()
{
    if (NULL != currentState)
    {
        this->currentState->Exit();
        this->currentState = NULL;
    }

    this->isRunning = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
StateManager::IsRunning()
{
    return this->isRunning;
}

} // namespace application

