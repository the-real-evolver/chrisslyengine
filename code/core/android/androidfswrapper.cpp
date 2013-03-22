//------------------------------------------------------------------------------
//  androidfswrapper.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "androidfswrapper.h"
#include "core/debug.h"

namespace chrissly
{

AAssetManager* AndroidFSWrapper::AssetManager = 0;

//------------------------------------------------------------------------------
/**
*/
core::FileHandle
AndroidFSWrapper::Open(const char* fileName, core::AccessMode flags, int mode)
{
    core::FileHandle fileHandle;
    fileHandle.handle = AAssetManager_open(AndroidFSWrapper::AssetManager, fileName, AASSET_MODE_UNKNOWN);
    CE_ASSERT(fileHandle.handle >= 0, "FSWrapper::Open(): can't open file '%s'\n", fileName);
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
    off_t fileSize = AAsset_getLength(fileHandle.handle);
    return (unsigned int)fileSize;
}

//------------------------------------------------------------------------------
/**
*/
int
AndroidFSWrapper::Read(core::FileHandle fileHandle, void* buf, unsigned int numBytes)
{
    int result = AAsset_read(fileHandle.handle, buf, (size_t)numBytes);
    CE_ASSERT(result >= 0, "FSWrapper::Read(): can't read from file '%i'\n", fileHandle.handle);
    return result;
}

//------------------------------------------------------------------------------
/**
*/
void
AndroidFSWrapper::_Initialise(AAssetManager* assetManager)
{
    AndroidFSWrapper::AssetManager = assetManager;
    CE_LOG("AndroidFSWrapper::_Initialise\n");
}

} // namespace chrissly
