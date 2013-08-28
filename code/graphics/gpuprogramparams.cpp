//------------------------------------------------------------------------------
//  gpuprogramparams.cpp
//  (C) 2013 Christian Bleicher
//------------------------------------------------------------------------------
#include "gpuprogramparams.h"

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
GpuNamedConstants::GpuNamedConstants()
{
    HashTableInit(&this->map, 4);
}

//------------------------------------------------------------------------------
/**
*/
GpuNamedConstants::~GpuNamedConstants()
{
    unsigned int i;
    for (i = 0; i < this->map.capacity; i++)
    {
        LinkedList* it = ((Chain*)DynamicArrayGet(&this->map.entries, i))->list;
        while (it != NULL)
        {
            CE_LOG("GpuNamedConstants::~GpuNamedConstants(): remove '%s'\n", ((KeyValuePair*)it->data)->key.C_Str());
            GpuConstantDefinition* def = (GpuConstantDefinition*)((KeyValuePair*)it->data)->value;
            CE_FREE(def->buffer);
            CE_DELETE def;
            it = it->next;
        }
    }

    HashTableClear(&this->map);
}

//------------------------------------------------------------------------------
/**
*/
GpuProgramParameters::GpuProgramParameters()
{
    this->constantDefs = NULL;
}

//------------------------------------------------------------------------------
/**
*/
GpuProgramParameters::~GpuProgramParameters()
{

}

//------------------------------------------------------------------------------
/**
*/
void
GpuProgramParameters::SetNamedConstant(const char* name, float val)
{
    GpuConstantDefinition* def = (GpuConstantDefinition*)HashTableFind(&this->constantDefs->map, name);
    if (def != NULL)
    {
        memcpy(def->buffer, &val, def->size);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GpuProgramParameters::SetNamedConstant(const char* name, int val)
{
    GpuConstantDefinition* def = (GpuConstantDefinition*)HashTableFind(&this->constantDefs->map, name);
    if (def != NULL)
    {
        memcpy(def->buffer, &val, def->size);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GpuProgramParameters::SetNamedConstant(const char* name, const chrissly::core::Matrix4& m)
{
    GpuConstantDefinition* def = (GpuConstantDefinition*)HashTableFind(&this->constantDefs->map, name);
    if (def != NULL)
    {
        memcpy(def->buffer, m[0], def->size);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GpuProgramParameters::_SetNamedConstants(GpuNamedConstants* constantmap)
{
    this->constantDefs = constantmap;
}

} // namespace graphics
} // namespace chrissly
