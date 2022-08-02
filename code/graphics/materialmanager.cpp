//------------------------------------------------------------------------------
//  materialmanager.cpp
//  (C) 2013 Christian Bleicher
//------------------------------------------------------------------------------
#include "materialmanager.h"
#include "memoryallocatorconfig.h"
#include "debug.h"

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

MaterialManager* MaterialManager::Singleton = NULL;

//------------------------------------------------------------------------------
/**
*/
MaterialManager::MaterialManager()
{
    Singleton = this;
    ce_hash_table_init(&this->resources, 4U);
    ce_hash_table_init(&this->scripts, 4U);
}

//------------------------------------------------------------------------------
/**
*/
MaterialManager::~MaterialManager()
{
    Singleton = NULL;
    this->RemoveAll();
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialManager::Initialise()
{
    this->ParseScript("materials.material");
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialManager::ParseScript(const char* const name)
{
    void* script = ce_hash_table_find(&this->scripts, name, strlen(name));
    if (script != NULL)
    {
        return;
    }

    this->parser.ParseScript(name);

    ce_hash_table_insert(&this->scripts, name, strlen(name), (void*)1U);
}

//------------------------------------------------------------------------------
/**
*/
Material* const
MaterialManager::Create(const char* const name)
{
    Material* material = (Material*)ce_hash_table_find(&this->resources, name, strlen(name));
    if (material != NULL)
    {
        CE_ASSERT(false, "MaterialManager::Create(): material '%s' already exists, duplicates are not allowed\n", name);
        return NULL;
    }

    material = CE_NEW Material();

    ce_hash_table_insert(&this->resources, name, strlen(name), material);

    return material;
}

//------------------------------------------------------------------------------
/**
*/
Material* const
MaterialManager::GetByName(const char* const name)
{
    return (Material*)ce_hash_table_find(&this->resources, name, strlen(name));
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialManager::RemoveAll()
{
    unsigned int i;
    for (i = 0U; i < this->resources.bucket_count; ++i)
    {
        ce_linked_list* it = this->resources.buckets[i];
        while (it != NULL)
        {
            CE_DELETE (Material*)((ce_key_value_pair*)it->data)->value;
            it = it->next;
        }
    }
    ce_hash_table_clear(&this->resources);

    ce_hash_table_clear(&this->scripts);
}

//------------------------------------------------------------------------------
/**
*/
ce_hash_table* const
MaterialManager::GetAllScripts() const
{
    return &this->scripts;
}

} // namespace graphics
} // namespace chrissly
