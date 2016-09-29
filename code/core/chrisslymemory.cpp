//------------------------------------------------------------------------------
//  chrisslymemory.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "chrisslymemory.h"

//------------------------------------------------------------------------------
/**
*/
void
ce_memory_fill_interleaved(void const* src0, void const* src1, void* dst, unsigned short stride, unsigned int dst_len)
{
    char* ptr_dst = (char*)dst;
    char const* ptr_src0 = (char const*)src0;
    char const* ptr_src1 = (char const*)src1;
    unsigned int double_stride = stride << 1;

    unsigned int i;
    while (dst_len)
    {
        for (i = 0; i < stride; ++i)
        {
            *ptr_dst = *ptr_src0++;
            *(ptr_dst + stride) = *ptr_src1++;
            ++ptr_dst;
        }
        ptr_dst += stride;
        dst_len -= double_stride;
    }
}