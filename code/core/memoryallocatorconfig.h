#ifndef MEMORYALLOCATORCONFIG_H_
#define MEMORYALLOCATORCONFIG_H_
//------------------------------------------------------------------------------
/**
    This file configures the engine's memory allocators.

    You can modify this file to alter the allocation routines used for the engine's main objects.

    (C) 2011 Christian Bleicher
*/
#include <malloc.h>

#define CE_NEW new
#define CE_DELETE delete
#define CE_MALLOC(bytes) malloc(bytes)
#define CE_MALLOC_ALIGN(align, bytes) memalign(align, bytes)
#define CE_FREE(ptr) free(ptr)
#define CE_REALLOC(ptr, size) realloc(ptr, size)

#endif