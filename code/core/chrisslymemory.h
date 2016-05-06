#ifndef CHRISSLYMEMORY_H_
#define CHRISSLYMEMORY_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::core::Memory

    Low level utility memory functions.

    (C) 2012 Christian Bleicher
*/
//------------------------------------------------------------------------------
namespace chrissly
{
namespace core
{

class Memory
{
public:
    /// fill a buffer interleaved with the given stride from 2 source buffers
    static void FillInterleaved(void const* src0, void const* src1, void* dst, unsigned short stride, unsigned int dstLen);
};

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#endif