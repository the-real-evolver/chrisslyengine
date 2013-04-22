#ifndef FILEHANDLE_H_
#define FILEHANDLE_H_
//------------------------------------------------------------------------------
/**
    Wrapper for a filehandle.
    
    02.06.2012 -> Creation
    
    (C) 2012 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __PSP__
namespace chrissly
{
namespace core
{

struct FileHandle
{
    /// default constructor
    FileHandle() : handle(0) {};

    int handle;
};

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __ANDROID__
#include <android/asset_manager.h>
namespace chrissly
{
namespace core
{

struct FileHandle
{
    /// default constructor
    FileHandle() : handle(0) {};

    AAsset* handle;
};

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "FileHandle struct not implemented on this platform!"
#endif
#endif
