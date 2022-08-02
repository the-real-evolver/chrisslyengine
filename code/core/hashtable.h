#ifndef HASHTABLE_H_
#define HASHTABLE_H_
//------------------------------------------------------------------------------
/**
    @file core/hashtable.h

    A simple hash table using separate chaining and byte buffers as keys.

    (C) 2012 Christian Bleicher (evolver)
*/
#include "linkedlist.h"
#include "chrisslyarray.h"
#include <stdint.h>

//------------------------------------------------------------------------------

/// hashtable
struct ce_hash_table
{
    ce_linked_list** buckets;
    unsigned int bucket_count;
    unsigned int size;
};

/// a key value pair
struct ce_key_value_pair
{
    char* key;
    unsigned int key_length;
    void* value;
};

/// forward declaration
static void ce_hash_table_resize(ce_hash_table* const table, unsigned int new_size);

//------------------------------------------------------------------------------
/**
*/
static unsigned int
ce_hash_function(const char* key, unsigned int key_length)
{
    // hashfunction: djb2
    unsigned int hash = 5381U;
    unsigned int i;
    for (i = 0U; i < key_length; ++i)
    {
        hash = ((hash << 5U) + hash) + *key++;
    }

    return hash;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ce_hash_table_init(ce_hash_table* const table, unsigned int initial_size)
{
    if (NULL == table)
    {
        return;
    }

    table->buckets = NULL;
    ce_array_init(table->buckets, initial_size);
    table->bucket_count = initial_size;
    table->size = 0U;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ce_hash_table_clear(ce_hash_table* const table)
{
    if (NULL == table)
    {
        return;
    }

    unsigned int i;
    for (i = 0U; i < table->bucket_count; ++i)
    {
        ce_linked_list* it = table->buckets[i];
        while (it != NULL)
        {
            ce_linked_list* node = it;
            ce_key_value_pair* kvp = (ce_key_value_pair*)node->data;
            CE_FREE(kvp->key);
            CE_FREE(kvp);
            it = it->next;
            ce_linked_list_remove(&table->buckets[i], node);
        }
    }

    ce_array_delete(table->buckets);
    table->bucket_count = 0U;
    table->size = 0U;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ce_hash_table_insert(ce_hash_table* const table, const char* const key, unsigned int key_length, void* const value)
{
    if (NULL == table || NULL == key || 0U == key_length)
    {
        return;
    }

    if (0U == table->bucket_count)
    {
        ce_hash_table_init(table, 1U);
    }

    if (table->size == table->bucket_count)
    {
        ce_hash_table_resize(table, table->bucket_count * 2U);
    }

    ce_key_value_pair* kvp = (ce_key_value_pair*)CE_MALLOC(sizeof(ce_key_value_pair));
    kvp->key = (char*)CE_MALLOC(key_length);
    kvp->key_length = key_length;
    memcpy(kvp->key, key, key_length);
    kvp->value = value;

    ce_linked_list** bucket = table->buckets + (uintptr_t)(ce_hash_function(key, key_length) % table->bucket_count);
    ce_linked_list_add(bucket, kvp);

    ++table->size;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ce_hash_table_erase(ce_hash_table* const table, void* const value)
{
    if (NULL == table)
    {
        return;
    }

    unsigned int i;
    for (i = 0U; i < table->bucket_count; ++i)
    {
        ce_linked_list* it = table->buckets[i];
        while (it != NULL)
        {
            ce_key_value_pair* kvp = (ce_key_value_pair*)it->data;
            if (kvp->value == value)
            {
                CE_FREE(kvp->key);
                CE_FREE(kvp);
                ce_linked_list* node = it;
                it = it->next;
                ce_linked_list_remove(&table->buckets[i], node);
                --table->size;
            }
            else
            {
                it = it->next;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
inline void*
ce_hash_table_find(ce_hash_table* const table, const char* const key, unsigned int key_length)
{
    if (NULL == table || NULL == key || 0U == key_length || 0U == table->bucket_count)
    {
        return NULL;
    }

    ce_linked_list* it = table->buckets[ce_hash_function(key, key_length) % table->bucket_count];
    while (it != NULL)
    {
        ce_key_value_pair* kvp = (ce_key_value_pair*)it->data;
        if (kvp->key_length == key_length)
        {
            if (0 == memcmp(key, kvp->key, key_length))
            {
                return kvp->value;
            }
        }
        it = it->next;
    }

    return NULL;
}

//------------------------------------------------------------------------------
/**
*/
inline ce_key_value_pair*
ce_hash_table_get_key(ce_hash_table* const table, void* const value)
{
    unsigned int i;
    for (i = 0U; i < table->bucket_count; ++i)
    {
        ce_linked_list* it = table->buckets[i];
        while (it != NULL)
        {
            ce_key_value_pair* kvp = (ce_key_value_pair*)it->data;
            if (kvp->value == value)
            {
                return kvp;
            }
            it = it->next;
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------
/**
*/
static void
ce_hash_table_resize(ce_hash_table* const table, unsigned int new_size)
{
    ce_hash_table new_table;
    ce_hash_table_init(&new_table, new_size);

    unsigned int i;
    for (i = 0U; i < table->bucket_count; ++i)
    {
        ce_linked_list* it = table->buckets[i];
        while (it != NULL)
        {
            ce_key_value_pair* kvp = (ce_key_value_pair*)it->data;
            ce_hash_table_insert(&new_table, kvp->key, kvp->key_length, kvp->value);
            it = it->next;
        }
    }

    ce_hash_table_clear(table);

    *table = new_table;
}

//------------------------------------------------------------------------------
#endif
