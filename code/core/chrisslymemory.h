#ifndef CHRISSLYMEMORY_H_
#define CHRISSLYMEMORY_H_
//------------------------------------------------------------------------------
/**
    @file core/chrisslymemory.h

    Low level utility memory functions.

    (C) 2012 Christian Bleicher
*/

/// fill a buffer interleaved with the given stride from 2 source buffers
void ce_memory_fill_interleaved(void const* const src0, void const* const src1, void* const dst, unsigned short stride, unsigned int dst_len);

#endif