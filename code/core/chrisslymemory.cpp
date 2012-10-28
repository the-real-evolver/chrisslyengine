//------------------------------------------------------------------------------
//  chrisslymemory.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "chrisslymemory.h"

namespace chrissly
{
namespace core
{

//------------------------------------------------------------------------------
/**
*/
void
Memory::FillInterleaved(void const* src0, void const* src1, void* dst, unsigned short stride, unsigned int dstLen)
{
    char* pDst = (char*)dst;
    char const* pSrc0 = (char const*)src0;
    char const* pSrc1 = (char const*)src1;
    unsigned int doubleStride = stride << 1;

    unsigned int i;
    while (dstLen)
    {
        for (i = 0; i < stride; i++)
        {
            *pDst = *pSrc0++;
            *(pDst + stride) = *pSrc1++;
            pDst++;
        }
        pDst += stride;
        dstLen -= doubleStride;
    }
}

} // namespace core
} // namespace chrissly