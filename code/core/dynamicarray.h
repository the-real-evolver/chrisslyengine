#ifndef DYNAMICARRAY_H_
#define DYNAMICARRAY_H_
//------------------------------------------------------------------------------
/**
    @file core/dynamicarray.h

    A dynamically resizing array for general use.

    (C) 2012 Christian Bleicher (evolver)
*/
#include "memoryallocatorconfig.h"
#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------

/// A resizable array
typedef struct ce_dynamic_array
{
    void** data;            //!< pointer to array of void pointers
    unsigned int capacity;  //!< storage space currently allocated for the array
    unsigned int size;      //!< number of actual objects held in the array, not necessarily equal to its storage capacity
} ce_dynamic_array;

//------------------------------------------------------------------------------
/**
    \brief Initializes an array with the supplied initial size
    \param v the array to initialize
    \param initialSize the initial size to allocate
    \return a pointer to the data, or NULL on error.
*/
inline void*
ce_dynamic_array_init(ce_dynamic_array* const v, unsigned int initial_size)
{
    if (NULL == v)
    {
        return NULL;
    }

    v->data = initial_size > 0U ? (void**)CE_CALLOC(initial_size, sizeof(void*)) : NULL;
    v->capacity = initial_size;
    v->size = 0;

    return v->data;
}

//------------------------------------------------------------------------------
/**
    \brief Frees memory allocated by the dynamic array
    \param v The array to delete.
*/
inline void
ce_dynamic_array_delete(ce_dynamic_array* const v)
{
    if (NULL == v)
    {
        return;
    }

    if (v->data != NULL)
    {
        CE_FREE(v->data);
        v->data = NULL;
    }

    v->capacity = 0U;
    v->size = 0U;
}

//------------------------------------------------------------------------------
/**
    \brief Gets the entry at the supplied index
    \param v The array to get from.
    \param index The index of the data to get.
    \return The data or NULL if v is NULL or the index is out of range.
*/
inline void*
ce_dynamic_array_get(ce_dynamic_array* const v, unsigned int index)
{
    if (NULL == v)
    {
        return NULL;
    }

    if (index >= v->capacity)
    {
        return NULL;
    }

    return v->data[index];
}

//------------------------------------------------------------------------------
/**
    \brief Sets the entry to the supplied value
    \param v The array to set.
    \param index The index of the data to set (array will be resized to fit the index).
    \param item The data to set.
    \return false if v is NULL or there isn't enough memory, true otherwise
*/
inline bool
ce_dynamic_array_set(ce_dynamic_array* const v, unsigned int index, void* const item)
{
    if (NULL == v)
    {
        return false;
    }

    if (index >= v->capacity)
    {
        // resize the array, making sure it is bigger than index.
        unsigned int new_size = (v->capacity * 2U > index ? v->capacity * 2U : index + 1U);

        void** temp = (void**)CE_REALLOC(v->data, sizeof(void*) * new_size);

        if (NULL == temp)
        {
            return false;
        }
        v->data = temp;
        memset(v->data + v->capacity, 0, sizeof(void*) * (new_size - v->capacity));
        v->capacity = new_size;
    }

    v->data[index] = item;

    return true;
}

//------------------------------------------------------------------------------
/**
    \brief Adds a new element at the end of the array, after its current last element
    \param v The array to set.
    \param item The data to set.
*/
inline void
ce_dynamic_array_push_back(ce_dynamic_array* const v, void* const item)
{
    if (NULL == v)
    {
        return;
    }

    ce_dynamic_array_set(v, v->size, item);

    ++v->size;
}

//------------------------------------------------------------------------------
#endif
