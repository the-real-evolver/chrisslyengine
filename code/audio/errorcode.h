#ifndef ERRORCODE_H_
#define ERRORCODE_H_
//------------------------------------------------------------------------------
/**
    Error codes. Returned from every function.

    (C) 2014 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly
{
namespace audio
{
    enum Result
    {
        /// No errors
        OK,
        /// A requested output, dsp unit type or codec was not available
        ERR_PLUGIN_MISSING,
        /// Error trying to allocate a channel
        ERR_CHANNEL_ALLOC,
        /// not enough memory or resources
        ERR_MEMORY
    };

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif