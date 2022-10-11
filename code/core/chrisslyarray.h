#ifndef CHRISSLYARRAY_H_
#define CHRISSLYARRAY_H_
//------------------------------------------------------------------------------
/**
    @file core/chrisslyarray.h

    Represents an array that can change in size. It's based on Sean Barrett's
    "stretchy buffers":
    https://github.com/nothings/stb/blob/master/deprecated/stretchy_buffer.h

    Arrays use contiguous storage locations for their items, which means
    that their items can also be accessed using offsets on regular pointers
    to its items. Their size can change dynamically, with their storage
    being handled automatically by the container.

    Internally, arrays use dynamically allocated memory to store their
    items. This memory may need to be reallocated in order to grow in size
    when new items are inserted, which implies allocating new memory and
    moving all items to it. This is a expensive task in terms of processing
    time, and thus, arrays do not reallocate each time an item is added to
    the container.

    Instead, arrays allocate some extra storage to accommodate for possible
    growth, and thus the container may have an actual capacity greater than the
    storage strictly needed to contain its items (i.e., its size).

    (C) 2017 Christian Bleicher (evolver)
*/
#include "memoryallocatorconfig.h"
#include <string.h>

/* forward declarations */
#ifdef __cplusplus
template<class T> static T* ce_array_grow_wrapper(T* a, unsigned int item_size, unsigned int n);
#else
#define ce_array_grow_wrapper ce_array_grow
#endif

/* Header placed in front of the array data. */
typedef struct ce_array_header_t
{
    unsigned int size;
    unsigned int capacity;
} ce_array_header_t;

/* Initialises a with the supplied initial size. */
#define ce_array_init(a, initial_size) \
    ((a) = NULL, (a) = ce_array_grow_wrapper(a, sizeof(*(a)), initial_size))

/* Deallocates all the storage capacity and sets a to zero */
#define ce_array_delete(a) \
    ((a) ? CE_FREE(ce_array_header(a)) : (void)0, (a) = NULL)

/* Returns the number of items of a. */
#define ce_array_size(a) \
    ((a) ? ce_array_header(a)->size : 0U)

/* Adds a new item at the end of the a, after its current last item. */
#define ce_array_push_back(a, item) \
    (ce_array_full(a) ? (a) = ce_array_grow_wrapper(a, sizeof(*(a)), 1U) : 0U, \
    (a)[ce_array_header(a)->size++] = (item))

/* Swap erase the given item. */
#define ce_array_erase(a, item) \
    do \
    { \
        if (a) \
        { \
            for (unsigned int i = 0U; i < ce_array_size(a); ++i) \
            { \
                if ((a)[i] == item) \
                { \
                    (a)[i] = a[ce_array_size(a) - 1U]; \
                    ce_array_header(a)->size--; \
                    break; \
                } \
            }\
        } \
    } while(0)

/* Returns the header data for a. */
#define ce_array_header(a) \
    ((ce_array_header_t*)((char*)(a) - sizeof(ce_array_header_t)))

/* Returns true if a needs to grow. */
#define ce_array_full(a) \
    ((a) ? ce_array_header(a)->size == ce_array_header(a)->capacity : 1U)

/* Grows the capacity of a to hold at least n more items. a is updated in-place. */
inline void*
ce_array_grow(void* a, unsigned int item_size, unsigned int n)
{
    unsigned int old_capacity = a ? ce_array_header(a)->capacity : 0U;
    unsigned int doubled_capacity = 2U * old_capacity;
    unsigned int requested_capacity = ce_array_size(a) + n;
    unsigned int capacity = doubled_capacity > requested_capacity ? doubled_capacity : requested_capacity;
    ce_array_header_t* new_memory = (ce_array_header_t*)CE_REALLOC(a ? ce_array_header(a) : 0U, item_size * capacity + sizeof(ce_array_header_t));
    if (new_memory)
    {
        if (!a) new_memory->size = 0U;
        new_memory->capacity = capacity;
        ++new_memory;
        memset((char*)(new_memory) + old_capacity * item_size, 0, (capacity - old_capacity) * item_size);
        return new_memory;
    }

    return NULL;
}

/* Calling ce_array_grow() directly won't work cause C++ doesn't allow implicit assignment of void* to T* */
#ifdef __cplusplus
template<class T> static T*
ce_array_grow_wrapper(T* a, unsigned int item_size, unsigned int n)
{
    return (T*)ce_array_grow((void*)a, item_size, n);
}
#endif

#endif
