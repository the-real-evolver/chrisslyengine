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
Material*
MaterialManager::Create(const char* name)
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
void
MaterialManager::RemoveAll()
{
    unsigned int i;
    for (i = 0; i < this->resources.capacity; i++)
    {
        LinkedList* it = ((Chain*)DynamicArrayGet(&this->resources.entries, i))->list;
        while (it != NULL)
        {
            Material* material = (Material*)((KeyValuePair*)it->data)->value;
            CE_DELETE material;
            it = it->next;
        }
    }

    HashTableClear(&this->resources);
}

} // namespace graphics
} // namespace chrissly
