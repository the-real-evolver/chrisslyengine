#ifndef HASHTABLE_H_
#define HASHTABLE_H_
//------------------------------------------------------------------------------
/**
    @file core/hashtable.h

    A simple hash table using separate chaining and strings as keys.

    (C) 2012 Christian Bleicher (evolver)
*/
#include "linkedlist.h"
#include "dynamicarray.h"

//------------------------------------------------------------------------------

/// hashtable
struct ce_hash_table
{
    ce_dynamic_array buckets;
    unsigned int bucket_count;
    unsigned int size;
};

/// buckets of the hashtable
struct ce_bucket
{
    ce_linked_list* list;
    unsigned int size;
};

/// a key value pair
struct ce_key_value_pair
{
    char* key;
    void* value;
};

/// forward declaration
static void ce_hash_table_resize(ce_hash_table* table, unsigned int new_size);

//------------------------------------------------------------------------------
/**
*/
static unsigned int
ce_hash_function(const char* key)
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
ce_hash_table_init(ce_hash_table* table, unsigned int initial_size)
{
    if (NULL == table)
    {
        return;
    }

    ce_dynamic_array_init(&table->buckets, initial_size);
    table->bucket_count = initial_size;
    table->size = 0;

    unsigned int i;
    for (i = 0; i < initial_size; ++i)
    {
        ce_bucket* bucket = (ce_bucket*)CE_MALLOC(sizeof(ce_bucket));
        bucket->size = 0;
        bucket->list = NULL;
        ce_dynamic_array_set(&table->buckets, i, bucket);
    }
}

//------------------------------------------------------------------------------
/**
*/
static inline void
ce_hash_table_clear(ce_hash_table* table)
{
    if (NULL == table)
    {
        return;
    }

    unsigned int i;
    for (i = 0; i < table->bucket_count; ++i)
    {
        ce_bucket* bucket = (ce_bucket*)ce_dynamic_array_get(&table->buckets, i);
        ce_linked_list* it = bucket->list;
        while (it != NULL)
        {
            ce_linked_list* node = it;
            ce_key_value_pair* kvp = (ce_key_value_pair*)node->data;
            CE_FREE(kvp->key);
            CE_FREE(kvp);
            it = it->next;
            ce_linked_list_remove(node);
        }
        CE_FREE(bucket);
    }

    ce_dynamic_array_delete(&table->buckets);
    table->bucket_count = 0;
    table->size = 0;
}

//------------------------------------------------------------------------------
/**
*/
static inline void
ce_hash_table_insert(ce_hash_table* table, const char* key, void* value)
{
    if (NULL == table)
    {
        return;
    }

    if (0 == table->bucket_count)
    {
        ce_hash_table_init(table, 1);
    }

    if (table->size == table->bucket_count)
    {
        ce_hash_table_resize(table, table->bucket_count * 2);
    }

    ce_key_value_pair* kvp = (ce_key_value_pair*)CE_MALLOC(sizeof(ce_key_value_pair));
    size_t length = strlen(key);
    kvp->key = (char*)CE_MALLOC(length + 1);
    strncpy(kvp->key, key, length);
    kvp->key[length] = '\0';
    kvp->value = value;

    ce_bucket* bucket = (ce_bucket*)ce_dynamic_array_get(&table->buckets, ce_hash_function(key) % table->bucket_count);
    ce_linked_list_add(&(bucket->list), kvp);
    ++bucket->size;

    ++table->size;
}

//------------------------------------------------------------------------------
/**
*/
static inline void*
ce_hash_table_find(ce_hash_table* table, const char* key)
{
    if (NULL == table)
    {
        return NULL;
    }

    if (0 == table->bucket_count)
    {
        return NULL;
    }

    ce_linked_list* it = ((ce_bucket*)ce_dynamic_array_get(&table->buckets, ce_hash_function(key) % table->bucket_count))->list;
    while (it != NULL)
    {
        if (0 == strcmp(key, ((ce_key_value_pair*)it->data)->key))
        {
            return ((ce_key_value_pair*)it->data)->value;
        }
        it = it->next;
    }

    return NULL;
}

//------------------------------------------------------------------------------
/**
*/
static inline ce_linked_list*
ce_hash_table_begin(ce_hash_table* table, unsigned int index)
{
    if (NULL == table)
    {
        return NULL;
    }

    return ((ce_bucket*)ce_dynamic_array_get(&table->buckets, index))->list;
}

//------------------------------------------------------------------------------
/**
*/
static void
ce_hash_table_resize(ce_hash_table* table, unsigned int new_size)
{
    ce_hash_table new_table;
    ce_hash_table_init(&new_table, new_size);

    unsigned int i;
    for (i = 0; i < table->bucket_count; ++i)
    {
        ce_linked_list* it = ((ce_bucket*)ce_dynamic_array_get(&table->buckets, i))->list;
        while (it != NULL)
        {
            ce_key_value_pair* kvp = (ce_key_value_pair*)it->data;
            ce_hash_table_insert(&new_table, kvp->key, kvp->value);
            it = it->next;
        }
    }

    ce_hash_table_clear(table);

    table->buckets = new_table.buckets;
    table->bucket_count = new_table.bucket_count;
    table->size = new_table.size;
}

//------------------------------------------------------------------------------
#endif
