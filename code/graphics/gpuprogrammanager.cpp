//------------------------------------------------------------------------------
//  gpuprogrammanager.cpp
//  (C) 2022 Christian Bleicher
//------------------------------------------------------------------------------
#include "gpuprogrammanager.h"
#include "miscutils.h"

namespace chrissly
{
namespace graphics
{

GpuProgramManager* GpuProgramManager::Singleton = NULL;

//------------------------------------------------------------------------------
/**
*/
GpuProgramManager::GpuProgramManager()
{
    Singleton = this;
    ce_hash_table_init(&this->resources, 4U);
}

//------------------------------------------------------------------------------
/**
*/
GpuProgramManager::~GpuProgramManager()
{
    Singleton = NULL;
    this->RemoveAll();
}

//------------------------------------------------------------------------------
/**
*/
GpuProgram*
GpuProgramManager::LoadFromSource(const char* const name, const char* const source, const char* const vertexShaderFunctionName, const char* const fragmentShaderFunctionName, const char* const* const macros)
{
    GpuProgram* program = (GpuProgram*)ce_hash_table_find(&this->resources, name, strlen(name));
    if (program != NULL)
    {
        return program;
    }

#if __CE_D3D11__
    program = CE_NEW GpuProgram(source, name, vertexShaderFunctionName, fragmentShaderFunctionName, macros);
    ce_hash_table_insert(&this->resources, name, strlen(name), program);
#endif

    return program;
}

//------------------------------------------------------------------------------
/**
*/
GpuProgram*
GpuProgramManager::LoadFromSource(const char *const name, const char *const vertexShaderSource, const char *const fragmentShaderSource, const char* const* const macros)
{
    GpuProgram* program = (GpuProgram*)ce_hash_table_find(&this->resources, name, strlen(name));
    if (program != NULL)
    {
        return program;
    }

#if __CE_GLES2__
    program = CE_NEW GpuProgram(vertexShaderSource, fragmentShaderSource, macros);
    ce_hash_table_insert(&this->resources, name, strlen(name), program);
#else
    CE_UNREFERENCED_PARAMETER(vertexShaderSource);
    CE_UNREFERENCED_PARAMETER(fragmentShaderSource);
    CE_UNREFERENCED_PARAMETER(macros);
#endif

    return program;
}

//------------------------------------------------------------------------------
/**
*/
GpuProgram*
GpuProgramManager::GetByName(const char* const name)
{
    return (GpuProgram*)ce_hash_table_find(&this->resources, name, strlen(name));
}

//------------------------------------------------------------------------------
/**
*/
void
GpuProgramManager::RemoveAll()
{
    unsigned int i;
    for (i = 0U; i < this->resources.bucket_count; ++i)
    {
        ce_linked_list* it = this->resources.buckets[i];
        while (it != NULL)
        {
            CE_DELETE(GpuProgram*)((ce_key_value_pair*)it->data)->value;
            it = it->next;
        }
    }
    ce_hash_table_clear(&this->resources);
}

} // namespace graphics
} // namespace chrissly
