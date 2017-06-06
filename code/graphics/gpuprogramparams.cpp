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
    ce_hash_table_init(&this->map, 4U);
}

//------------------------------------------------------------------------------
/**
*/
GpuNamedConstants::~GpuNamedConstants()
{
    unsigned int i;
    for (i = 0U; i < this->map.bucket_count; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(&this->map, i);
        while (it != NULL)
        {
            CE_LOG("GpuNamedConstants::~GpuNamedConstants(): remove constant definition\n");
            GpuConstantDefinition* def = (GpuConstantDefinition*)((ce_key_value_pair*)it->data)->value;
            CE_FREE(def->buffer);
            CE_DELETE def;
            it = it->next;
        }
    }

    ce_hash_table_clear(&this->map);
}

//------------------------------------------------------------------------------
/**
*/
GpuProgramParameters::GpuProgramParameters() :
    constantDefs(NULL)
{
    unsigned int i;
    for (i = 0U; i < ACT_COUNT; ++i)
    {
        this->autoConstants[i] = NULL;
    }
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
    GpuConstantDefinition* def = (GpuConstantDefinition*)ce_hash_table_find(&this->constantDefs->map, name, strlen(name));
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
    GpuConstantDefinition* def = (GpuConstantDefinition*)ce_hash_table_find(&this->constantDefs->map, name, strlen(name));
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
    GpuConstantDefinition* def = (GpuConstantDefinition*)ce_hash_table_find(&this->constantDefs->map, name, strlen(name));
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
    GpuConstantDefinition* def = (GpuConstantDefinition*)ce_hash_table_find(&this->constantDefs->map, name, strlen(name));
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
    GpuConstantDefinition* def = (GpuConstantDefinition*)ce_hash_table_find(&this->constantDefs->map, name, strlen(name));
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
    GpuConstantDefinition* def = (GpuConstantDefinition*)ce_hash_table_find(&this->constantDefs->map, name, strlen(name));
    if (def != NULL)
    {
        memcpy(def->buffer, m[0U], def->size);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GpuProgramParameters::SetNamedConstant(const char* name, const Matrix4* m, unsigned int numEntries)
{
    GpuConstantDefinition* def = (GpuConstantDefinition*)ce_hash_table_find(&this->constantDefs->map, name, strlen(name));
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
GpuProgramParameters::SetAutoConstant(AutoConstantType autoType, const Matrix4& m)
{
    CE_ASSERT(autoType >= ACT_WORLD_MATRIX && autoType < ACT_COUNT, "GpuProgramParameters::SetAutoConstant(): invalid AutoConstantType value '%u'\n", autoType);
    GpuConstantDefinition* def = this->autoConstants[autoType];
    if (def != NULL)
    {
        CE_ASSERT(GCT_MATRIX_4X4 == def->constType, "GpuProgramParameters::SetAutoConstant(): auto constant '%u' is not of type 'Matrix4'\n", autoType);
        memcpy(def->buffer, m[0U], def->size);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GpuProgramParameters::SetAutoConstant(AutoConstantType autoType, float val)
{
    CE_ASSERT(autoType >= ACT_WORLD_MATRIX && autoType < ACT_COUNT, "GpuProgramParameters::SetAutoConstant(): invalid AutoConstantType value '%u'\n", autoType);
    GpuConstantDefinition* def = this->autoConstants[autoType];
    if (def != NULL)
    {
        CE_ASSERT(GCT_FLOAT1 == def->constType, "GpuProgramParameters::SetAutoConstant(): auto constant '%u' is not of type 'float'\n", autoType);
        memcpy(def->buffer, &val, def->size);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
GpuProgramParameters::_SetNamedConstants(GpuNamedConstants* constantMap)
{
    this->constantDefs = constantMap;
}

//------------------------------------------------------------------------------
/**
*/
void
GpuProgramParameters::_SetAutoConstant(AutoConstantType autoType, GpuConstantDefinition* def)
{
    CE_ASSERT(autoType >= ACT_WORLD_MATRIX && autoType < ACT_COUNT, "GpuProgramParameters::_SetAutoConstant(): invalid AutoConstantType value '%u'\n", autoType);
    this->autoConstants[autoType] = def;
}

//------------------------------------------------------------------------------
/**
*/
GpuProgramParameters::AutoConstantType
GpuProgramParameters::AutoConstantTypeFromString(const char* name)
{
    if (0 == strcmp(name, "worldMatrix"))           {return ACT_WORLD_MATRIX;}
    if (0 == strcmp(name, "viewMatrix"))            {return ACT_VIEW_MATRIX;}
    if (0 == strcmp(name, "projectionMatrix"))      {return ACT_PROJECTION_MATRIX;}
    if (0 == strcmp(name, "worldViewProjMatrix"))   {return ACT_WORLDVIEWPROJ_MATRIX;}
    if (0 == strcmp(name, "morphWeight"))           {return ACT_MORPH_WEIGHT;}

    return ACT_COUNT;
}

} // namespace graphics
} // namespace chrissly
