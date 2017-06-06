#ifndef WINAPIMUTEX_H_
#define WINAPIMUTEX_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::WinAPIMutex

    (C) 2016 Christian Bleicher
*/
#include "debug.h"
#include <windows.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class WinAPIMutex
{
public:
    /// constructor
    WinAPIMutex();
    /// destructor
    ~WinAPIMutex();
    /// lock mutex
    void Lock() const;
    /// unlock mutex
    void Unlock() const;

private:
    HANDLE mutex;
};

//------------------------------------------------------------------------------
/**
*/
inline
WinAPIMutex::WinAPIMutex()
{
    this->mutex = CreateMutex(
        NULL,   /* default security attributes  */
        FALSE,  /* initially not owned          */
        NULL    /* unnamed mutex                */
    );
    CE_ASSERT(this->mutex != NULL, "WinAPIMutex::WinAPIMutex(): failed to create mutex\n");
}

//------------------------------------------------------------------------------
/**
*/
inline
WinAPIMutex::~WinAPIMutex()
{
#if __DEBUG__
    BOOL result =
#endif
    CloseHandle(this->mutex);
    CE_ASSERT(result != FALSE, "WinAPIMutex::~WinAPIMutex(): failed to close handle '%p'\n", this->mutex);
}

//------------------------------------------------------------------------------
/**
*/
inline void
WinAPIMutex::Lock() const
{
#if __DEBUG__
    DWORD state =
#endif
    WaitForSingleObject(this->mutex, INFINITE);
    CE_ASSERT(WAIT_OBJECT_0 == state, "WinAPIMutex::Lock(): failed to lock mutex '%p'\n", this->mutex);
}

//------------------------------------------------------------------------------
/**
*/
inline void
WinAPIMutex::Unlock() const
{
#if __DEBUG__
    BOOL result =
#endif
    ReleaseMutex(this->mutex);
    CE_ASSERT(result != FALSE, "WinAPIMutex::Unlock(): failed to unlock mutex '%p'\n", this->mutex);
}

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
