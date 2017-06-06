//------------------------------------------------------------------------------
//  winapifswrapper.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "winapifswrapper.h"
#include "debug.h"
#include <stdio.h>

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
core::FileHandle
WinAPIFSWrapper::Open(const char* fileName, core::AccessMode flags, core::AccessPattern pattern, int mode)
{
    CE_UNREFERENCED_PARAMETER(pattern);
    CE_UNREFERENCED_PARAMETER(mode);

    core::FileHandle fileHandle;
    fileHandle.handle = CreateFile(
        fileName,                       /* _In_     LPCTSTR lpFileName                          */
        WinAPIFSWrapper::Get(flags),    /* _In_     DWORD dwDesiredAccess                       */
        FILE_SHARE_READ,                /* _In_     DWORD dwShareMode                           */
        NULL,                           /* _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes  */
        OPEN_EXISTING,                  /* _In_     DWORD dwCreationDisposition                 */
        FILE_ATTRIBUTE_NORMAL,          /* _In_     DWORD dwFlagsAndAttributes                  */
        NULL                            /* _In_opt_ HANDLE hTemplateFile                        */
    );
    CE_ASSERT(fileHandle.handle != INVALID_HANDLE_VALUE, "FSWrapper::Open(): failed to open file '%s'\n", fileName);

    return fileHandle;
}

//------------------------------------------------------------------------------
/**
*/
void
WinAPIFSWrapper::Close(core::FileHandle fileHandle)
{
#if __DEBUG__
    BOOL result =
#endif
    CloseHandle(fileHandle.handle);
    CE_ASSERT(result != FALSE, "FSWrapper::Close(): failed to close file '%p'\n", fileHandle.handle);
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
WinAPIFSWrapper::GetFileSize(core::FileHandle fileHandle)
{
    LARGE_INTEGER fileSize;

#if __DEBUG__
    BOOL result =
#endif
    GetFileSizeEx(fileHandle.handle, &fileSize);
    CE_ASSERT(result != FALSE, "FSWrapper::GetFileSize(): failed to get filesize of file '%p'\n", fileHandle.handle);

    return (unsigned int)fileSize.QuadPart;
}

//------------------------------------------------------------------------------
/**
*/
int
WinAPIFSWrapper::Read(core::FileHandle fileHandle, void* buf, unsigned int numBytes)
{
    DWORD bytesRead;

#if __DEBUG__
    BOOL result =
#endif
    ReadFile(
        fileHandle.handle,  /* _In_         HANDLE hFile                */
        buf,                /* _Out_        LPVOID lpBuffer             */
        (DWORD)numBytes,    /* _In_         DWORD nNumberOfBytesToRead  */
        &bytesRead,         /* _Out_opt_    LPDWORD lpNumberOfBytesRead */
        NULL                /* _Inout_opt_  LPOVERLAPPED lpOverlapped   */
    );
    CE_ASSERT(result != FALSE, "FSWrapper::Read(): failed to read from file '%p'\n", fileHandle.handle);

    return (int)bytesRead;
}

//------------------------------------------------------------------------------
/**
*/
void
WinAPIFSWrapper::Seek(core::FileHandle fileHandle, int offset, core::SeekOrigin whence)
{
#if __DEBUG__
    DWORD result =
#endif
    SetFilePointer(
        fileHandle.handle,              /* _In_         HANDLE hFile                */
        (LONG)offset,                   /* _In_         LONG lDistanceToMove        */
        NULL,                           /* _Inout_opt_  PLONG lpDistanceToMoveHigh  */
        WinAPIFSWrapper::Get(whence)    /* _In_         DWORD dwMoveMethod          */
    );
    CE_ASSERT(result != INVALID_SET_FILE_POINTER, "FSWrapper::Seek(): failed to seek file '%p'\n", fileHandle.handle);
}

//------------------------------------------------------------------------------
/**
*/
DWORD
WinAPIFSWrapper::Get(core::AccessMode mode)
{
    switch (mode)
    {
        case core::ReadAccess:      return GENERIC_READ;
        case core::WriteAccess:     return GENERIC_WRITE;
        case core::ReadWriteAccess: return GENERIC_READ | GENERIC_WRITE;
        case core::AppendAccess:    return GENERIC_ALL;
        default: CE_ASSERT(false, "FSWrapper::Get(): illegal AccessMode '%i'\n", mode);
    }

    return 0U;
}

//------------------------------------------------------------------------------
/**
*/
DWORD
WinAPIFSWrapper::Get(core::SeekOrigin origin)
{
    switch (origin)
    {
        case core::Begin:   return FILE_BEGIN;
        case core::Current: return FILE_CURRENT;
        case core::End:     return FILE_END;
        default: CE_ASSERT(false, "FSWrapper::Get(): illegal SeekOrigin '%i'\n", origin);
    }

    return 0U;
}

} // namespace chrissly
