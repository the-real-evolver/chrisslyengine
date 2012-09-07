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

const char* PSPFSWrapper::APP_EXPORT_PATH = "ms0:/PSP/GAME/ChrisslyEngine_PSP/export/";

//------------------------------------------------------------------------------
/**
*/
core::FileHandle
PSPFSWrapper::Open(const char* fileName, core::AccessMode flags, int mode)
{
    char filePath[128];
    strcpy(filePath, APP_EXPORT_PATH);
    strcat(filePath, fileName);
    core::FileHandle fileHandle;
    fileHandle.handle = sceIoOpen(filePath, PSPFSWrapper::Get(flags), mode);
    CE_ASSERT(fileHandle.handle >= 0, "FSWrapper::Open(): can't open file '%s'\n", fileName);
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
PSPFSWrapper::Read(core::FileHandle fileHandle, void* buf, unsigned int numBytes)
{
    int result = sceIoRead(fileHandle.handle, buf, numBytes);
    CE_ASSERT(result >= 0, "FSWrapper::Read(): can't read from file '%i'\n", fileHandle.handle);
    return result;
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
        case core::WriteAccess:     return PSP_O_WRONLY;
        case core::ReadWriteAccess: return PSP_O_RDWR;
        case core::AppendAccess:    return PSP_O_APPEND;
        default: CE_ASSERT(false, "FSWrapper::Get(): illegal AccessMode '%i'\n", mode);
    }
    
    return 0;
}

} // namespace chrissly