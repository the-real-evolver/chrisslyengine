#ifndef ANDROIDFSWRAPPER_H_
#define ANDROIDFSWRAPPER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::AndroidFSWrapper

    Wrapper for the android assetmanager and internal storage filesystem.

    (C) 2012 Christian Bleicher
*/
#include "core/fileio.h"
#include "core/filehandle.h"
#include <android/native_activity.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class AndroidFSWrapper
{
public:
    /// open a file
    static core::FileHandle Open(const char* const fileName, core::AccessMode mode, core::AccessPattern pattern, int permission);
    /// close a file
    static void Close(core::FileHandle fileHandle);
    /// get size of a file in bytes
    static unsigned int GetFileSize(core::FileHandle fileHandle);
    /// read number of bytes from a file, returns the total number of elements successfully read
    static int Read(core::FileHandle fileHandle, void* const buf, unsigned int numBytes);
    /// seek in a file
    static void Seek(core::FileHandle fileHandle, int offset, core::SeekOrigin whence);
    /// write to a file
    static void Write(core::FileHandle fileHandle, const void* buf, unsigned int numBytes);
    /// return true if a file exists
    static bool FileExists(const char* const fileName);
    /// delete a file
    static bool RemoveFile(const char* const fileName);
    /// list all files in a directory, returns the number of files in the list
    static int ListFiles(const char* const path, const char* const pattern, unsigned int maxNumFiles, char filesOut[][260U]);
    /// return internal application private storage data path
    static const char* GetAppDataDirectory();
    /// initialise the filesystem
    static void _Initialise(ANativeActivity* const nativeActivity);

private:
    /// return a Posix equivalent for a AccessMode value
    static const char* Get(core::AccessMode mode);
    /// return a Posix equivalent for a SeekOrigin value
    static int Get(core::SeekOrigin origin);
    /// return a android asset mode equivalent for a AccessPattern value
    static int Get(core::AccessPattern pattern);
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
