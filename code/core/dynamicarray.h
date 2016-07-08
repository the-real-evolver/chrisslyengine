/*---------------------------------------------------------------------------------

    A simple vector like dynamic data structure

  Copyright (C) 2005
            Jason Rogers (dovoto)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.


---------------------------------------------------------------------------------*/
/*! \file dynamicArray.h
    \brief A dynamically resizing array for general use.
*/

#ifndef __DYNAMICARRAY_H__
#define __DYNAMICARRAY_H__

#include "memoryallocatorconfig.h"
#include <stdio.h>
#include <string.h>

//! A resizable array
typedef struct DynamicArray
{
    void** data;             //!< pointer to array of void pointers
    unsigned int capacity;  //!< storage space currently allocated for the array
    unsigned int size;      //!< number of actual objects held in the array, not necessarily equal to its storage capacity
} DynamicArray;



/*! \brief Initializes an array with the supplied initial size
    \param v the array to initialize
    \param initialSize the initial size to allocate
    \return a pointer to the data, or NULL on error.
*/
static inline void*
DynamicArrayInit(DynamicArray* v, unsigned int initialSize)
{
    if (NULL == v)
    {
        return NULL;
    }

    v->data = (void**)CE_MALLOC(sizeof(void*) * initialSize);
    v->capacity = initialSize;
    v->size = 0;

    return v->data;
}


/*! \brief Frees memory allocated by the dynamic array
    \param v The array to delete.
*/
static inline void
DynamicArrayDelete(DynamicArray* v)
{
    if (NULL == v)
    {
        return;
    }

    if (v->data != NULL)
    {
        CE_FREE(v->data);
    }

    v->data = NULL;
    v->capacity = 0;
    v->size = 0;
}


/*! \brief Gets the entry at the supplied index
    \param v The array to get from.
    \param index The index of the data to get.
    \return The data or NULL if v is NULL or the index is out of range.
*/
static inline void*
DynamicArrayGet(DynamicArray* v, unsigned int index)
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


/*! \brief Sets the entry to the supplied value
    \param v The array to set.
    \param index The index of the data to set (array will be resized to fit the index).
    \param item The data to set.
    \return false if v is NULL or there isn't enough memory, true otherwise
*/
static inline bool
DynamicArraySet(DynamicArray* v, unsigned int index, void* item)
{
    if (NULL == v)
    {
        return false;
    }

    if (index >= v->capacity)
    {
        // resize the array, making sure it is bigger than index.
        unsigned int newSize = (v->capacity * 2 > index ? v->capacity * 2: index + 1);

        void** temp = (void**)CE_REALLOC(v->data, sizeof(void*) * newSize);

        if (NULL == temp) return false;
        v->data = temp;
        memset(v->data + v->capacity, 0, sizeof(void*) * (newSize - v->capacity));
        v->capacity = newSize;
    }

    v->data[index] = item;
    return true;
}

/*! \brief Adds a new element at the end of the array, after its current last element
    \param v The array to set.
    \param item The data to set.
*/
static inline void
DynamicArrayPushBack(DynamicArray* v, void* item)
{
    if (NULL == v)
    {
        return;
    }

    DynamicArraySet(v, v->size, item);

    ++v->size;
}

#endif
