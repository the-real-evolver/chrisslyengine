//------------------------------------------------------------------------------
//  winapifswrapper.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "winapifswrapper.h"
#include "miscutils.h"
#include "debug.h"
#include <stdio.h>
#include <shlobj_core.h>

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
core::FileHandle
WinAPIFSWrapper::Open(const char* const fileName, core::AccessMode mode, core::AccessPattern pattern, int permission)
{
    CE_UNREFERENCED_PARAMETER(pattern);
    CE_UNREFERENCED_PARAMETER(permission);

    core::FileHandle fileHandle;
    fileHandle.handle = CreateFile(
        fileName,                                   /* _In_     LPCTSTR lpFileName                          */
        WinAPIFSWrapper::Get(mode),                 /* _In_     DWORD dwDesiredAccess                       */
        FILE_SHARE_READ,                            /* _In_     DWORD dwShareMode                           */
        NULL,                                       /* _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes  */
        WinAPIFSWrapper::GetDispositionFlag(mode),  /* _In_     DWORD dwCreationDisposition                 */
        FILE_ATTRIBUTE_NORMAL,                      /* _In_     DWORD dwFlagsAndAttributes                  */
        NULL                                        /* _In_opt_ HANDLE hTemplateFile                        */
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
#if __CE_DEBUG__
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

#if __CE_DEBUG__
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
WinAPIFSWrapper::Read(core::FileHandle fileHandle, void* const buf, unsigned int numBytes)
{
    DWORD bytesRead;

#if __CE_DEBUG__
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
    CE_ASSERT((bytesRead != 0U && numBytes == bytesRead) || bytesRead == 0U, "FSWrapper::Read(): could not read the requested number of bytes (requested: '%d' read: '%d')\n", numBytes, bytesRead);

    return (int)bytesRead;
}

//------------------------------------------------------------------------------
/**
*/
void
WinAPIFSWrapper::Seek(core::FileHandle fileHandle, int offset, core::SeekOrigin whence)
{
#if __CE_DEBUG__
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
void
WinAPIFSWrapper::Write(core::FileHandle fileHandle, const void* buf, unsigned int numBytes)
{
    DWORD bytesWritten;
#if __CE_DEBUG__
    BOOL result =
#endif
    WriteFile(
        fileHandle.handle,  /* _In_                                         HANDLE hFile                    */
        buf,                /* _In_reads_bytes_opt_(nNumberOfBytesToWrite)  LPCVOID lpBuffer                */
        (DWORD)numBytes,    /* _In_                                         DWORD nNumberOfBytesToWrite     */
        &bytesWritten,      /* _Out_opt_                                    LPDWORD lpNumberOfBytesWritten  */
        NULL                /* _Inout_opt_                                  LPOVERLAPPED lpOverlapped       */
    );
    CE_ASSERT(result != FALSE, "FSWrapper::Write(): failed to write to file '%p'\n", fileHandle.handle);
}

//------------------------------------------------------------------------------
/**
    See: https://devblogs.microsoft.com/oldnewthing/20071023-00/?p=24713
*/
bool
WinAPIFSWrapper::FileExists(const char* const fileName)
{
    DWORD fileAttr = GetFileAttributes(fileName);
    if (0xffffffff == fileAttr || 0U != (FILE_ATTRIBUTE_DIRECTORY & fileAttr)) return false;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
const char*
WinAPIFSWrapper::GetAppDataDirectory()
{
    static TCHAR AppDataDirectory[MAX_PATH] = {'\0'};
    PWSTR path = NULL;
#if __CE_DEBUG__
    HRESULT result =
#endif
    SHGetKnownFolderPath(
        FOLDERID_RoamingAppData,    /* [in]           REFKNOWNFOLDERID rfid      */
        0U,                         /* [in]           DWORD            dwFlags   */
        NULL,                       /* [in, optional] HANDLE           hToken    */
        &path                       /* [out]          PWSTR * ppszPath           */
    );
    CE_ASSERT(SUCCEEDED(result), "FSWrapper::GetAppDataDirectory(): failed to get app data directory\n");

    wcstombs(AppDataDirectory, path, MAX_PATH);

    CoTaskMemFree(path);

    return AppDataDirectory;
}

//------------------------------------------------------------------------------
/**
*/
const char*
WinAPIFSWrapper::GetBinDirectory()
{
    static TCHAR BinDirectory[MAX_PATH] = {'\0'};
#if __CE_DEBUG__
    DWORD result =
#endif
    GetModuleFileName(
        NULL,           /* [in, optional] HMODULE hModule       */
        BinDirectory,   /* [out]          LPSTR   lpFilename    */
        MAX_PATH        /* [in]           DWORD   nSize         */
    );
    CE_ASSERT(result != MAX_PATH, "FSWrapper::GetBinDirectory(): failed to get bin directory\n");

    char* lastSlash = strrchr(BinDirectory, '\\');
    if (lastSlash != NULL) *lastSlash = '\0';

    return BinDirectory;
}

//------------------------------------------------------------------------------
/**
*/
void
WinAPIFSWrapper::ChangeWorkingDirectory(const char* const path)
{
#if __CE_DEBUG__
    BOOL result =
#endif
    SetCurrentDirectory(path);
    CE_ASSERT(result != FALSE, "FSWrapper::ChangeWorkingDirectory(): failed to change working directory\n");
}

//------------------------------------------------------------------------------
/**
*/
DWORD
WinAPIFSWrapper::Get(core::AccessMode mode)
{
    switch (mode)
    {
        case core::READ_ACCESS:         return GENERIC_READ;
        case core::WRITE_ACCESS:        return GENERIC_WRITE;
        case core::READ_WRITE_ACCESS:   return GENERIC_READ | GENERIC_WRITE;
        case core::APPEND_ACCESS:       return FILE_APPEND_DATA;
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
        case core::BEGIN:   return FILE_BEGIN;
        case core::CURRENT: return FILE_CURRENT;
        case core::END:     return FILE_END;
        default: CE_ASSERT(false, "FSWrapper::Get(): illegal SeekOrigin '%i'\n", origin);
    }

    return 0U;
}

//------------------------------------------------------------------------------
/**
*/
DWORD
WinAPIFSWrapper::GetDispositionFlag(core::AccessMode mode)
{
    switch (mode)
    {
        case core::READ_ACCESS:         return OPEN_EXISTING;
        case core::WRITE_ACCESS:        return CREATE_ALWAYS;
        case core::READ_WRITE_ACCESS:
        case core::APPEND_ACCESS:       return OPEN_ALWAYS;
        default: CE_ASSERT(false, "FSWrapper::GetDispositionFlag(): illegal AccessMode '%i'\n", mode);
    }

    return 0U;
}

} // namespace chrissly
