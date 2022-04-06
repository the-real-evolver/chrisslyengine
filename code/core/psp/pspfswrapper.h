#ifndef PSPFSWRAPPER_H_
#define PSPFSWRAPPER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::PSPFSWrapper

    Wrapper for the sce filesystem.

    (C) 2011 Christian Bleicher
*/
#include "fileio.h"
#include "filehandle.h"

//------------------------------------------------------------------------------
namespace chrissly
{

class PSPFSWrapper
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
    /// return a PSP equivalent for a AccessMode value
    static int Get(core::AccessMode mode);
    /// return a PSP equivalent for a SeekOrigin value
    static int Get(core::SeekOrigin origin);
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
