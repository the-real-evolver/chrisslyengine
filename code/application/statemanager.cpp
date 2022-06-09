//------------------------------------------------------------------------------
//  statemanager.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "statemanager.h"
#include <stdio.h>
#include "debug.h"

namespace chrissly
{
namespace application
{

StateManager* StateManager::Singleton = NULL;

//------------------------------------------------------------------------------
/**
*/
StateManager::StateManager() :
    previousState(NULL),
    currentState(NULL),
    nextState(NULL),
    isRunning(false)
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
StateManager::Trigger()
{
    if (this->currentState != NULL)
    {
        this->currentState->Trigger();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
StateManager::ChangeState(State* const state)
{
    this->nextState = state;
    this->previousState = this->currentState;
    if (state != this->currentState)
    {
        if (this->currentState != NULL)
        {
            this->currentState->Exit();
        }

        CE_ASSERT(state != NULL, "StateManager::ChangeState() pointer to state is NULL\n");
        this->currentState = state;
        this->currentState->Enter();
        this->isRunning = true;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
StateManager::Exit()
{
    if (this->currentState != NULL)
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
StateManager::IsRunning() const
{
    return this->isRunning;
}

//------------------------------------------------------------------------------
/**
*/
State const* const
StateManager::GetPreviousState() const
{
    return this->previousState;
}

//------------------------------------------------------------------------------
/**
*/
State const* const
StateManager::GetNextState() const
{
    return this->nextState;
}

} // namespace application
} // namespace chrissly
