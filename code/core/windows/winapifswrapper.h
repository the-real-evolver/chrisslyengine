#ifndef WINAPIFSWRAPPER_H_
#define WINAPIFSWRAPPER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::WinAPIFSWrapper

    Wrapper for the WinAPI filesystem.

    (C) 2016 Christian Bleicher
*/
#include "core/fileio.h"
#include "core/filehandle.h"

//------------------------------------------------------------------------------
namespace chrissly
{

class WinAPIFSWrapper
{
public:
    /// open a file
    static core::FileHandle Open(const char* const fileName, core::AccessMode mode, core::AccessPattern pattern, int permission);
    /// close a file
    static void Close(core::FileHandle fileHandle);
    /// get size of a file in bytes
    static unsigned int GetFileSize(core::FileHandle fileHandle);
    /// read from a file
    static int Read(core::FileHandle fileHandle, void* const buf, unsigned int numBytes);
    /// seek in a file
    static void Seek(core::FileHandle fileHandle, int offset, core::SeekOrigin whence);
    /// return true if a file exists
    static bool FileExists(const char* const fileName);

private:
    /// return a WinAPI equivalent for a AccessMode value
    static DWORD Get(core::AccessMode mode);
    /// return a WinAPI equivalent for a SeekOrigin value
    static DWORD Get(core::SeekOrigin origin);
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
