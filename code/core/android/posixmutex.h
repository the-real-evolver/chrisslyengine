#ifndef POSIXMUTEX_H_
#define POSIXMUTEX_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::PosixMutex

    (C) 2015 Christian Bleicher
*/
#include "debug.h"
#include <pthread.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class PosixMutex
{
public:
    /// constructor
    PosixMutex();
    /// destructor
    ~PosixMutex();
    /// lock mutex
    void Lock();
    /// unlock mutex
    void Unlock();

private:
    pthread_mutex_t mutex;
};

//------------------------------------------------------------------------------
/**
*/
inline
PosixMutex::PosixMutex()
{
    pthread_mutexattr_t attrs;
    pthread_mutexattr_init(&attrs);
    pthread_mutexattr_settype(&attrs, PTHREAD_MUTEX_RECURSIVE);
    int result = pthread_mutex_init(&this->mutex, &attrs);
    CE_ASSERT(0 == result, "PosixMutex::PosixMutex(): pthread_mutex_init failed: %08x\n", result);
    pthread_mutexattr_destroy(&attrs);
}

//------------------------------------------------------------------------------
/**
*/
inline
PosixMutex::~PosixMutex()
{
    int result = pthread_mutex_destroy(&this->mutex);
    CE_ASSERT(0 == result, "PosixMutex::~PosixMutex(): pthread_mutex_destroy failed: %08x\n", result);
}

//------------------------------------------------------------------------------
/**
*/
inline void
PosixMutex::Lock()
{
    pthread_mutex_lock(&this->mutex);
}

//------------------------------------------------------------------------------
/**
*/
inline void
PosixMutex::Unlock()
{
    pthread_mutex_unlock(&this->mutex);
}

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
