#ifndef FILEIO_H_
#define FILEIO_H_
//------------------------------------------------------------------------------
/**
    06.02.2012 -> Creation
    
    (C) 2012 Christian Bleicher
*/
//------------------------------------------------------------------------------
namespace chrissly
{
namespace core
{
    /// access modes
    enum AccessMode
    {
        ReadAccess,
        WriteAccess,
        AppendAccess,
        ReadWriteAccess
    };

    /// access pattern
    enum AccessPattern
    {
        // no specific information about how data will be accessed
        Unknown,
        // read chunks and seek forward and backward
        Random,
        // read sequentially, with an occasional forward seek
        Streaming,
        // caller plans to ask for a read-only buffer with all data
        Buffer
    };

    /// seek origins
    enum SeekOrigin
    {
        Begin,
        Current,
        End
    };

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
