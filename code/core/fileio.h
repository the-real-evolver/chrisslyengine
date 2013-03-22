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
