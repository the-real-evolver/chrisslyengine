//------------------------------------------------------------------------------
//  androidfswrapper.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "androidfswrapper.h"
#include "core/debug.h"
#include <stdio.h>

namespace chrissly
{

AAssetManager* AndroidFSWrapper::AssetManager = NULL;

//------------------------------------------------------------------------------
/**
*/
core::FileHandle
AndroidFSWrapper::Open(const char* const fileName, core::AccessMode mode, core::AccessPattern pattern, int permission)
{
    core::FileHandle fileHandle;
    fileHandle.handle = AAssetManager_open(AndroidFSWrapper::AssetManager, fileName, AndroidFSWrapper::Get(pattern));
    CE_ASSERT(fileHandle.handle != NULL, "FSWrapper::Open(): can't open file '%s'\n", fileName);
    return fileHandle;
}

//------------------------------------------------------------------------------
/**
*/
void
AndroidFSWrapper::Close(core::FileHandle fileHandle)
{
    AAsset_close(fileHandle.handle);
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
AndroidFSWrapper::GetFileSize(core::FileHandle fileHandle)
{
    return (unsigned int)AAsset_getLength(fileHandle.handle);
}

//------------------------------------------------------------------------------
/**
*/
int
AndroidFSWrapper::Read(core::FileHandle fileHandle, void* const buf, unsigned int numBytes)
{
    int result = AAsset_read(fileHandle.handle, buf, numBytes);
    CE_ASSERT(result >= 0, "FSWrapper::Read(): can't read from file '%i'\n", fileHandle.handle);
    return result;
}

//------------------------------------------------------------------------------
/**
*/
void
AndroidFSWrapper::Seek(core::FileHandle fileHandle, int offset, core::SeekOrigin whence)
{
    off_t result = AAsset_seek(fileHandle.handle, (off_t)offset, AndroidFSWrapper::Get(whence));
    CE_ASSERT(result != -1, "FSWrapper::Seek(): failed to seek file '%i'\n", fileHandle.handle);
}

//------------------------------------------------------------------------------
/**
*/
void
AndroidFSWrapper::_Initialise(AAssetManager* const assetManager)
{
    AndroidFSWrapper::AssetManager = assetManager;
    CE_LOG("AndroidFSWrapper::_Initialise\n");
}

//------------------------------------------------------------------------------
/**
*/
int
AndroidFSWrapper::Get(core::SeekOrigin origin)
{
    switch (origin)
    {
        case core::BEGIN:   return SEEK_SET;
        case core::CURRENT: return SEEK_CUR;
        case core::END:     return SEEK_END;
        default: CE_ASSERT(false, "FSWrapper::Get(): illegal SeekOrigin '%i'\n", origin);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
AndroidFSWrapper::Get(core::AccessPattern pattern)
{
    switch (pattern)
    {
        case core::UNKNOWN:   return AASSET_MODE_UNKNOWN;
        case core::RANDOM:    return AASSET_MODE_RANDOM;
        case core::STREAMING: return AASSET_MODE_STREAMING;
        case core::BUFFER:    return AASSET_MODE_BUFFER;
        default: CE_ASSERT(false, "FSWrapper::Get(): illegal AccessPattern '%i'\n", pattern);
    }

    return -1;
}

} // namespace chrissly
