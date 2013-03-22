#ifndef STATE_H_
#define STATE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::application::State

    11.06.2007 -> Creation

    (C) 2010 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly
{
namespace application
{

class State
{
public:
    /// default constructor
    State() {};
    /// destructor
    virtual ~State() {};
    /// called when entering the state
    virtual void Enter() = 0;
    /// called when leaving the state
    virtual void Exit() = 0;
    /// trigger the state
    virtual void Trigger() = 0;
};

} // namespace application
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
