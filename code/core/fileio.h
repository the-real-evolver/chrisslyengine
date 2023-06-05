#ifndef FILEIO_H_
#define FILEIO_H_
//------------------------------------------------------------------------------
/**
    File IO related enumerations.

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
        READ_ACCESS,
        WRITE_ACCESS,
        APPEND_ACCESS,
        READ_WRITE_ACCESS
    };

    /// access pattern
    enum AccessPattern
    {
        // no specific information about how data will be accessed
        UNKNOWN,
        // read chunks and seek forward and backward
        RANDOM,
        // read sequentially, with an occasional forward seek
        STREAMING,
        // caller plans to ask for a read-only buffer with all data
        BUFFER
    };

    /// seek origins
    enum SeekOrigin
    {
        BEGIN,
        CURRENT,
        END
    };

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
