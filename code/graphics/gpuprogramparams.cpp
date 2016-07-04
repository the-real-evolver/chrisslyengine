//------------------------------------------------------------------------------
//  gpuprogramparams.cpp
//  (C) 2013 Christian Bleicher
//------------------------------------------------------------------------------
#include "gpuprogramparams.h"
#include "debug.h"

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
    for (i = 0; i < this->map.bucketCount; ++i)
    {
        LinkedList* it = HashTableBegin(&this->map, i);
        while (it != NULL)
        {
            CE_LOG("GpuNamedConstants::~GpuNamedConstants(): remove '%s'\n", ((KeyValuePair*)it->data)->key);
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
GpuProgramParameters::SetNamedConstant(const char* name, const Vector3& vec)
{
    GpuConstantDefinition* def = (GpuConstantDefinition*)HashTableFind(&this->constantDefs->map, name);
    if (def != NULL)
    {
        memcpy(def->buffer, &vec.x, def->size);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GpuProgramParameters::SetNamedConstant(const char* name, const Vector3* vec, unsigned int numEntries)
{
    GpuConstantDefinition* def = (GpuConstantDefinition*)HashTableFind(&this->constantDefs->map, name);
    if (def != NULL)
    {
        CE_ASSERT(numEntries <= def->arraySize, "GpuProgramParameters::SetNamedConstant(): Vector3 numEntries: '%u' > arraySize: '%u'\n", numEntries, def->arraySize);
        memcpy(def->buffer, vec, def->size * numEntries);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GpuProgramParameters::SetNamedConstant(const char* name, const Quaternion& q)
{
    GpuConstantDefinition* def = (GpuConstantDefinition*)HashTableFind(&this->constantDefs->map, name);
    if (def != NULL)
    {
        memcpy(def->buffer, &q.w, def->size);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GpuProgramParameters::SetNamedConstant(const char* name, const Matrix4& m)
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
GpuProgramParameters::SetNamedConstant(const char* name, const Matrix4* m, unsigned int numEntries)
{
    GpuConstantDefinition* def = (GpuConstantDefinition*)HashTableFind(&this->constantDefs->map, name);
    if (def != NULL)
    {
        CE_ASSERT(numEntries <= def->arraySize, "GpuProgramParameters::SetNamedConstant(): Matrix4 numEntries: '%u' > arraySize: '%u'\n", numEntries, def->arraySize);
        memcpy(def->buffer, m, def->size * numEntries);
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
