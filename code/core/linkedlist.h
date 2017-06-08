#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_
//------------------------------------------------------------------------------
/**
    @file core/linkedlist.h

    A simple doubly linked, unsorted list implementation.

    (C) 2012 Christian Bleicher (evolver)
*/
#include "memoryallocatorconfig.h"
#include <stdio.h>

//------------------------------------------------------------------------------

/// A node for the linked list.
typedef struct ce_linked_list
{
    struct ce_linked_list* next;    //!< A pointer to the next node.
    struct ce_linked_list* prev;    //!< A pointer to the previous node.
    void* data;                     //!< A pointer to some data.
} ce_linked_list;

//------------------------------------------------------------------------------
/**
    \brief Adds data to a linked list.

    This will only store the pointer to the data, so you have to make sure that the pointer stays valid.

    \param front A pointer to a pointer to the front of the linked list (or a pointer to NULL if you don't have a linked list yet).
    \param data A pointer to the data you want to store.
*/
inline void
ce_linked_list_add(ce_linked_list** front, void* const data)
{
    ce_linked_list* node = (ce_linked_list*)CE_MALLOC(sizeof(ce_linked_list));

    node->prev = NULL;
    node->data = data;

    if (NULL == *front)
    {
        node->next = NULL;
    }
    else
    {
        node->next = *front;
        (*front)->prev = node;
    }

    *front = node;
}

//------------------------------------------------------------------------------
/**
    \brief Removes a node from a linked list.

    The data pointer of the node will be lost after this, so make sure you don't need it anymore.

    \param node The node you want to remove.
*/
inline void
ce_linked_list_remove(ce_linked_list* const node)
{
    if (NULL == node)
    {
        return;
    }

    if (node->prev != NULL)
    {
        node->prev->next = node->next;
    }

    if (node->next != NULL)
    {
        node->next->prev = node->prev;
    }

    CE_FREE(node);
}

//------------------------------------------------------------------------------
#endif
