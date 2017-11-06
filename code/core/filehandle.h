#ifndef FILEHANDLE_H_
#define FILEHANDLE_H_
//------------------------------------------------------------------------------
/**
    Wrapper for a filehandle.

    (C) 2012 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __PSP__
#include <pspiofilemgr.h>
namespace chrissly
{
namespace core
{

struct FileHandle
{
    /// default constructor
    FileHandle() : handle(-1) {};

    SceUID handle;
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
    FileHandle() : handle(NULL) {};

    AAsset* handle;
};

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __WINDOWS__
#include <windows.h>
namespace chrissly
{
namespace core
{

struct FileHandle
{
    /// default constructor
    FileHandle() : handle(INVALID_HANDLE_VALUE) {};

    HANDLE handle;
};

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#else
#error "FileHandle struct not implemented on this platform!"
#endif
#endif
