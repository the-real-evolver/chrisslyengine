#ifndef TIMER_H_
#define TIMER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::core::Timer

    (C) 2014 Christian Bleicher
*/
#include <time.h>

//------------------------------------------------------------------------------
namespace chrissly
{
namespace core
{

class Timer
{
public:
    /// constructor
    Timer();
    /// start/continue the timer
    void Start();
    /// stop the timer
    void Stop();
    /// reset the timer
    void Reset();
    /// return true if currently running
    bool Running() const;
    /// get current time in seconds
    double GetTime() const;

private:
    /// return internal time as 64 bit integer
    clock_t InternalTime() const;

    bool running;
    clock_t diffTime;
    clock_t stopTime;
};

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#endif

