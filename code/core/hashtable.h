#ifndef HASHTABLE_H_
#define HASHTABLE_H_
//------------------------------------------------------------------------------
/**
    A simple hash table using separate chaining and strings as keys.

    (C) 2012 Christian Bleicher (evolver)

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
*/
#include "linkedlist.h"
#include "dynamicarray.h"

/// hashtable
struct HashTable
{
    DynamicArray entries;
    unsigned int capacity;
    unsigned int currentSize;
};

/// entries of the hashtable
struct Chain
{
    LinkedList* list;
    unsigned int size;
};

/// a key value pair
struct KeyValuePair
{
    const char* key;
    void* value;
};

/// forward declaration
static inline void HashTableResize(HashTable* table, unsigned int newSize);

//------------------------------------------------------------------------------
/**
*/
static unsigned int
HashFunction(const char* key)
{
    // hashfunction: djb2
    unsigned int hash = 5381;
    int c;
    while ((c = *key++))
    {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

//------------------------------------------------------------------------------
/**
*/
static inline void
HashTableInit(HashTable* table, unsigned int initialSize)
{
    DynamicArrayInit(&table->entries, initialSize);
    table->currentSize = 0;
    table->capacity = initialSize;

    unsigned int i;
    for (i = 0; i < initialSize; i++)
    {
        Chain* chain = (Chain*)CE_MALLOC(sizeof(Chain));
        chain->size = 0;
        chain->list = NULL;
        DynamicArraySet(&table->entries, i, chain);
    }
}

//------------------------------------------------------------------------------
/**
*/
static inline void
HashTableClear(HashTable* table)
{
    unsigned int i;
    for (i = 0; i < table->capacity; i++)
    {
        Chain* chain = (Chain*)DynamicArrayGet(&table->entries, i);
        LinkedList* it = chain->list;
        while (it != NULL)
        {
            LinkedList* node = it;
            CE_FREE((KeyValuePair*)node->data);
            it = it->next;
            linkedlistRemove(node);
        }
        CE_FREE(chain);
    }

    DynamicArrayDelete(&table->entries);
    table->capacity = 0;
    table->currentSize = 0;
}

//------------------------------------------------------------------------------
/**
*/
static inline void
HashTableInsert(HashTable* table, const char* key, void* value)
{
    if (0 == table->capacity)
    {
        HashTableInit(table, 1);
    }

    if (table->currentSize == table->capacity)
    {
        HashTableResize(table, table->capacity * 2);
    }

    unsigned int hash = HashFunction(key);
    unsigned int index = hash % table->capacity;

    KeyValuePair* keyValuePair = (KeyValuePair*)CE_MALLOC(sizeof(KeyValuePair));
    keyValuePair->key = key;
    keyValuePair->value = value;

    Chain* chain = (Chain*)DynamicArrayGet(&table->entries, index);
    chain->list = linkedlistAdd(&(chain->list), keyValuePair);
    chain->list->data = keyValuePair;
    chain->size++;

    table->currentSize++;
}

//------------------------------------------------------------------------------
/**
*/
static inline void*
HashTableFind(HashTable* table, const char* key)
{
    unsigned int hash = HashFunction(key);
    unsigned int index = hash % table->capacity;

    LinkedList* it = ((Chain*)DynamicArrayGet(&table->entries, index))->list;
    while (it != NULL)
    {
        if (0 == strcmp(key, ((KeyValuePair*)it->data)->key))
        {
            return ((KeyValuePair*)it->data)->value;
        }
        it = it->next;
    }

    return NULL;
}

//------------------------------------------------------------------------------
/**
*/
static inline void
HashTableResize(HashTable* table, unsigned int newSize)
{
    HashTable newTable;
    HashTableInit(&newTable, newSize);

    unsigned int i;
    for (i = 0; i < table->capacity; i++)
    {
        LinkedList* it = ((Chain*)DynamicArrayGet(&table->entries, i))->list;
        while (it != NULL)
        {
            KeyValuePair* kvp = (KeyValuePair*)it->data;
            HashTableInsert(&newTable, kvp->key, kvp->value);
            it = it->next;   
        }
    }

    HashTableClear(table);

    table->entries = newTable.entries;
    table->capacity = newTable.capacity;
    table->currentSize = newTable.currentSize;
}
#endif
