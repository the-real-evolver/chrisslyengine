//------------------------------------------------------------------------------
//  materialmanager.cpp
//  (C) 2013 Christian Bleicher
//------------------------------------------------------------------------------
#include "materialmanager.h"
#include "memoryallocatorconfig.h"

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
    this->parser.ParseScript("materials.material");
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialManager::ParseScript(const char* const name)
{
    this->parser.ParseScript(name);
}

//------------------------------------------------------------------------------
/**
*/
Material* const
MaterialManager::CreateOrRetrieve(const char* const name)
{
    Material* material = (Material*)ce_hash_table_find(&this->resources, name, strlen(name));
    if (material != NULL)
    {
        return material;
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
        ce_linked_list* it = ce_hash_table_begin(&this->resources, i);
        while (it != NULL)
        {
            CE_DELETE (Material*)((ce_key_value_pair*)it->data)->value;
            it = it->next;
        }
    }

    ce_hash_table_clear(&this->resources);
}

} // namespace graphics
} // namespace chrissly
