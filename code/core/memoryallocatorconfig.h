#ifndef MEMORYALLOCATORCONFIG_H_
#define MEMORYALLOCATORCONFIG_H_
//------------------------------------------------------------------------------
/**
    This file configures the engine's memory allocators.

    You can modify this file to alter the allocation routines used for the
    engine's main objects.

    (C) 2011 Christian Bleicher
*/
#include <malloc.h>

#define CE_CACHE_LINE_SIZE 64U

#define CE_NEW new
#define CE_DELETE delete

#if __WINDOWS__
#define CE_MALLOC(bytes) _aligned_malloc(bytes, 2U)
#define CE_MALLOC_ALIGN(align, bytes) _aligned_malloc(bytes, align)
#define CE_CALLOC(num, size) _aligned_malloc(num * size, 2U)
#define CE_REALLOC(ptr, size) _aligned_realloc(ptr, size, 2U)
#define CE_FREE(ptr) _aligned_free(ptr)
#else
#define CE_MALLOC(bytes) malloc(bytes)
#define CE_MALLOC_ALIGN(align, bytes) memalign(align, bytes)
#define CE_CALLOC(num, size) calloc(num, size)
#define CE_REALLOC(ptr, size) realloc(ptr, size)
#define CE_FREE(ptr) free(ptr)
#endif

#endif