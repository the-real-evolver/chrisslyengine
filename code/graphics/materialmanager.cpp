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
    HashTableInit(&this->resources, 4);
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
MaterialManager::ParseScript(const char* name)
{
    this->parser.ParseScript(name);
}

//------------------------------------------------------------------------------
/**
*/
Material*
MaterialManager::CreateOrRetrieve(const char* name)
{
    Material* material = (Material*)HashTableFind(&this->resources, name);
    if (material != NULL)
    {
        return material;
    }

    material = CE_NEW Material();

    HashTableInsert(&this->resources, name, material);

    return material;
}

//------------------------------------------------------------------------------
/**
*/
Material*
MaterialManager::GetByName(const char* name)
{
    return (Material*)HashTableFind(&this->resources, name);
}

//------------------------------------------------------------------------------
/**
*/
void
MaterialManager::RemoveAll()
{
    unsigned int i;
    for (i = 0; i < this->resources.capacity; ++i)
    {
        LinkedList* it = ((Chain*)DynamicArrayGet(&this->resources.entries, i))->list;
        while (it != NULL)
        {
            CE_DELETE (Material*)((KeyValuePair*)it->data)->value;
            it = it->next;
        }
    }

    HashTableClear(&this->resources);
}

} // namespace graphics
} // namespace chrissly
