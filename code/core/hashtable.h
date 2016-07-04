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

//------------------------------------------------------------------------------

/// hashtable
struct HashTable
{
    DynamicArray buckets;
    unsigned int bucketCount;
    unsigned int currentSize;
};

/// buckets of the hashtable
struct Bucket
{
    LinkedList* list;
    unsigned int size;
};

/// a key value pair
struct KeyValuePair
{
    char* key;
    void* value;
};

/// forward declaration
static void HashTableResize(HashTable* table, unsigned int newSize);

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
    if (NULL == table)
    {
        return;
    }

    DynamicArrayInit(&table->buckets, initialSize);
    table->bucketCount = initialSize;
    table->currentSize = 0;

    unsigned int i;
    for (i = 0; i < initialSize; ++i)
    {
        Bucket* bucket = (Bucket*)CE_MALLOC(sizeof(Bucket));
        bucket->size = 0;
        bucket->list = NULL;
        DynamicArraySet(&table->buckets, i, bucket);
    }
}

//------------------------------------------------------------------------------
/**
*/
static inline void
HashTableClear(HashTable* table)
{
    if (NULL == table)
    {
        return;
    }

    unsigned int i;
    for (i = 0; i < table->bucketCount; ++i)
    {
        Bucket* bucket = (Bucket*)DynamicArrayGet(&table->buckets, i);
        LinkedList* it = bucket->list;
        while (it != NULL)
        {
            LinkedList* node = it;
            KeyValuePair* kvp = (KeyValuePair*)node->data;
            CE_FREE(kvp->key);
            CE_FREE(kvp);
            it = it->next;
            LinkedlistRemove(node);
        }
        CE_FREE(bucket);
    }

    DynamicArrayDelete(&table->buckets);
    table->bucketCount = 0;
    table->currentSize = 0;
}

//------------------------------------------------------------------------------
/**
*/
static inline void
HashTableInsert(HashTable* table, const char* key, void* value)
{
    if (NULL == table)
    {
        return;
    }

    if (0 == table->bucketCount)
    {
        HashTableInit(table, 1);
    }

    if (table->currentSize == table->bucketCount)
    {
        HashTableResize(table, table->bucketCount * 2);
    }

    KeyValuePair* keyValuePair = (KeyValuePair*)CE_MALLOC(sizeof(KeyValuePair));
    size_t length = strlen(key);
    keyValuePair->key = (char*)CE_MALLOC(length + 1);
    strncpy(keyValuePair->key, key, length);
    keyValuePair->key[length] = '\0';
    keyValuePair->value = value;

    Bucket* bucket = (Bucket*)DynamicArrayGet(&table->buckets, HashFunction(key) % table->bucketCount);
    LinkedlistAdd(&(bucket->list), keyValuePair);
    ++bucket->size;

    ++table->currentSize;
}

//------------------------------------------------------------------------------
/**
*/
static inline void*
HashTableFind(HashTable* table, const char* key)
{
    if (NULL == table)
    {
        return NULL;
    }

    if (0 == table->bucketCount)
    {
        return NULL;
    }

    LinkedList* it = ((Bucket*)DynamicArrayGet(&table->buckets, HashFunction(key) % table->bucketCount))->list;
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
static inline LinkedList*
HashTableBegin(HashTable* table, unsigned int index)
{
    if (NULL == table)
    {
        return NULL;
    }

    return ((Bucket*)DynamicArrayGet(&table->buckets, index))->list;
}

//------------------------------------------------------------------------------
/**
*/
static void
HashTableResize(HashTable* table, unsigned int newSize)
{
    HashTable newTable;
    HashTableInit(&newTable, newSize);

    unsigned int i;
    for (i = 0; i < table->bucketCount; ++i)
    {
        LinkedList* it = ((Bucket*)DynamicArrayGet(&table->buckets, i))->list;
        while (it != NULL)
        {
            KeyValuePair* kvp = (KeyValuePair*)it->data;
            HashTableInsert(&newTable, kvp->key, kvp->value);
            it = it->next;
        }
    }

    HashTableClear(table);

    table->buckets = newTable.buckets;
    table->bucketCount = newTable.bucketCount;
    table->currentSize = newTable.currentSize;
}

//------------------------------------------------------------------------------
#endif
