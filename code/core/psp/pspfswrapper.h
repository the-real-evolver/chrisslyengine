#ifndef PSPFSWRAPPER_H_
#define PSPFSWRAPPER_H_
//------------------------------------------------------------------------------
/**
    wrapper for the sce filesystem
    
    10.08.2011 -> Creation
    
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
    static core::FileHandle Open(const char* fileName, core::AccessMode flags, int mode);
    /// close a file
    static void Close(core::FileHandle fileHandle);
    /// get size of a file in bytes
    static unsigned int GetFileSize(core::FileHandle fileHandle);
    /// read from a file
    static int Read(core::FileHandle fileHandle, void* buf, unsigned int numBytes);
    
private:
    /// return a PSP equivalent for a AccessMode value
    static int Get(core::AccessMode mode);
    
    static const char* APP_EXPORT_PATH;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
