#ifndef ANDROIDFSWRAPPER_H_
#define ANDROIDFSWRAPPER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::AndroidFSWrapper

    Wrapper for the android assetmanager filesystem.

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
    static core::FileHandle Open(const char* fileName, core::AccessMode flags, core::AccessPattern pattern, int mode);
    /// close a file
    static void Close(core::FileHandle fileHandle);
    /// get size of a file in bytes
    static unsigned int GetFileSize(core::FileHandle fileHandle);
    /// read from a file
    static int Read(core::FileHandle fileHandle, void* buf, unsigned int numBytes);
    /// seek in a file
    static void Seek(core::FileHandle fileHandle, int offset, core::SeekOrigin whence);
    /// initialise the filesystem
    static void _Initialise(AAssetManager* assetManager);

private:
    /// return a Posix equivalent for a SeekOrigin value
    static int Get(core::SeekOrigin origin);
    /// return a android asset mode equivalent for a AccessPattern value
    static int Get(core::AccessPattern pattern);

    static AAssetManager* AssetManager;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
