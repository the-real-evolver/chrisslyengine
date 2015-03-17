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
    return (unsigned int)AAsset_getLength(fileHandle.handle);
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
AndroidFSWrapper::Seek(core::FileHandle fileHandle, int offset, core::SeekOrigin whence)
{
    off_t result = AAsset_seek(fileHandle.handle, (off_t)offset, AndroidFSWrapper::Get(whence));
    CE_ASSERT(result != -1, "FSWrapper::Seek(): failed to seek file '%i'\n", fileHandle.handle);
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

//------------------------------------------------------------------------------
/**
*/
int
AndroidFSWrapper::Get(core::SeekOrigin origin)
{
    switch (origin)
    {
        case core::Begin:   return SEEK_SET;
        case core::Current: return SEEK_CUR;
        case core::End:     return SEEK_END;
        default: CE_ASSERT(false, "FSWrapper::Get(): illegal SeekOrigin '%i'\n", origin);
    }

    return 0;
}

} // namespace chrissly
