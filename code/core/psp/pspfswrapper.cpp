//------------------------------------------------------------------------------
//  pspfswrapper.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "pspfswrapper.h"
#include <string.h>
#include <stdio.h>
#include <pspkernel.h>
#include "debug.h"

namespace chrissly
{

#ifdef ROOT_DIR
static const char* const RootDirectory = ROOT_DIR;
#else
static const char* const RootDirectory = "ms0:/PSP/GAME/ChrisslyEngine_PSP/export/";
#endif

//------------------------------------------------------------------------------
/**
*/
core::FileHandle
PSPFSWrapper::Open(const char* const fileName, core::AccessMode mode, core::AccessPattern pattern, int permission)
{
    char filePath[128U] = {'\0'};
    CE_ASSERT(strlen(RootDirectory) + strlen(fileName) < 128U, "FSWrapper::Open(): file path too long (limit is 128 characters) '%s%s'\n", RootDirectory, fileName);
    strcpy(filePath, RootDirectory);
    strcat(filePath, fileName);
    core::FileHandle fileHandle;
    fileHandle.handle = sceIoOpen(filePath, PSPFSWrapper::Get(mode), permission);
    CE_ASSERT(fileHandle.handle >= 0, "FSWrapper::Open(): can't open file '%s'\n", filePath);
    return fileHandle;
}

//------------------------------------------------------------------------------
/**
*/
void
PSPFSWrapper::Close(core::FileHandle fileHandle)
{
    int result = sceIoClose(fileHandle.handle);
    CE_ASSERT(result >= 0, "FSWrapper::Close(): can't close file '%i'\n", fileHandle.handle);
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
PSPFSWrapper::GetFileSize(core::FileHandle fileHandle)
{
    unsigned int fileSize = sceIoLseek(fileHandle.handle, 0, SEEK_END);
    sceIoLseek(fileHandle.handle, 0, SEEK_SET);
    return fileSize;
}

//------------------------------------------------------------------------------
/**
*/
int
PSPFSWrapper::Read(core::FileHandle fileHandle, void* const buf, unsigned int numBytes)
{
    int result = sceIoRead(fileHandle.handle, buf, numBytes);
    CE_ASSERT(result >= 0, "FSWrapper::Read(): can't read from file '%i'\n", fileHandle.handle);
    return result;
}

//------------------------------------------------------------------------------
/**
*/
void
PSPFSWrapper::Seek(core::FileHandle fileHandle, int offset, core::SeekOrigin whence)
{
    sceIoLseek(fileHandle.handle, (SceOff)offset, PSPFSWrapper::Get(whence));
}

//------------------------------------------------------------------------------
/**
*/
void
PSPFSWrapper::Write(core::FileHandle fileHandle, const void* buf, unsigned int numBytes)
{
    int bytesWritten = sceIoWrite(fileHandle.handle, buf, numBytes);
    CE_ASSERT((unsigned int)bytesWritten == numBytes, "FSWrapper::Write(): failed to write to file '%i', bytes supplied '%i', bytes written '%i'\n", fileHandle.handle, numBytes, bytesWritten);
}

//------------------------------------------------------------------------------
/**
*/
bool
PSPFSWrapper::FileExists(const char* const fileName)
{
    char filePath[128U] = {'\0'};
    CE_ASSERT(strlen(RootDirectory) + strlen(fileName) < 128U, "FSWrapper::FileExists(): file path too long (limit is 128 characters) '%s%s'\n", RootDirectory, fileName);
    strcpy(filePath, RootDirectory);
    strcat(filePath, fileName);
    SceUID handle = sceIoOpen(filePath, PSP_O_RDONLY, 0777);
    if (handle < 0) return false;
    (void)sceIoClose(handle);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
int
PSPFSWrapper::Get(core::AccessMode mode)
{
    switch (mode)
    {
        case core::ReadAccess:      return PSP_O_RDONLY;
        case core::WriteAccess:     return PSP_O_WRONLY | PSP_O_CREAT;
        case core::ReadWriteAccess: return PSP_O_RDWR | PSP_O_CREAT;
        case core::AppendAccess:    return PSP_O_APPEND | PSP_O_CREAT | PSP_O_WRONLY;
        default: CE_ASSERT(false, "FSWrapper::Get(): illegal AccessMode '%i'\n", mode);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
PSPFSWrapper::Get(core::SeekOrigin origin)
{
    switch (origin)
    {
        case core::Begin:   return PSP_SEEK_SET;
        case core::Current: return PSP_SEEK_CUR;
        case core::End:     return PSP_SEEK_END;
        default: CE_ASSERT(false, "FSWrapper::Get(): illegal SeekOrigin '%i'\n", origin);
    }

    return 0;
}

} // namespace chrissly