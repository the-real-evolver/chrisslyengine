//------------------------------------------------------------------------------
//  timer.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "timer.h"

#if __CE_PSP__
#undef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 1000000
#endif

namespace chrissly
{
namespace core
{

//------------------------------------------------------------------------------
/**
*/
static inline clock_t
ce_clock()
{
#if __CE_ANDROID__
    struct timespec ts = {};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000L + ts.tv_nsec / 1000L;
#else
    return clock();
#endif
}

//------------------------------------------------------------------------------
/**
*/
Timer::Timer() :
    running(false),
    diffTime(0),
    stopTime(0)
{

}

//------------------------------------------------------------------------------
/**
*/
void
Timer::Start()
{
    clock_t curRealTime;
    curRealTime = ce_clock();
    this->diffTime += curRealTime - this->stopTime;
    this->stopTime = 0;
    this->running = true;
}

//------------------------------------------------------------------------------
/**
*/
void
Timer::Stop()
{
    this->stopTime = ce_clock();
    this->running = false;
}

//------------------------------------------------------------------------------
/**
*/
void
Timer::Reset()
{
    this->stopTime = 0;
    this->diffTime = 0;
    if (this->running)
    {
        this->Start();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
Timer::Running() const
{
    return this->running;
}

//------------------------------------------------------------------------------
/**
*/
clock_t
Timer::InternalTime() const
{
    clock_t inttime;
    if (this->running)
    {
        inttime = ce_clock();
    }
    else
    {
        inttime = this->stopTime;
    }

    inttime -= this->diffTime;

    return inttime;
}

//------------------------------------------------------------------------------
/**
*/
double
Timer::GetTime() const
{
    return ((double)this->InternalTime()) / ((double)CLOCKS_PER_SEC);
}

} // namespace core
} // namespace chrissly

