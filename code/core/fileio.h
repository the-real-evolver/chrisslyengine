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
        ReadWriteAccess,
    };
} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
