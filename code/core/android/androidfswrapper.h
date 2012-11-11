#ifndef ANDROIDFSWRAPPER_H_
#define ANDROIDFSWRAPPER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::AndroidFSWrapper

    wrapper for the android assetmanager filesystem

    17.05.2012 -> Creation

    (C) 2012 Christian Bleicher
*/
#include "core/fileio.h"
#include "core/filehandle.h"
#include <android/asset_manager.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class AndroidFSWrapper
{
public:
    /// open a file
    static core::FileHandle Open(const char* fileName, core::AccessMode flags, int mode);
    /// close a file
    static void Close(core::FileHandle fileHandle);
    /// get size of a file in bytes
    static unsigned int GetFileSize(core::FileHandle fileHandle);
    /// read from a file
    static int Read(core::FileHandle fileHandle, void* buf, unsigned int numBytes);
    /// initialise the filesystem
    static void _Initialise(AAssetManager* assetManager);

private:
    static AAssetManager* assetManager;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
