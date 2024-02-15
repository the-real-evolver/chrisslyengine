#ifndef GPUPROGRAMMANAGER_H_
#define GPUPROGRAMMANAGER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::GpuProgramManager

    (C) 2022 Christian Bleicher
*/
#include "gpuprogram.h"
#include "hashtable.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class GpuProgramManager
{
public:
    /// get pointer to the singleton
    static GpuProgramManager* const Instance()
    {
        return Singleton;
    }

    /// default constructor
    GpuProgramManager();
    /// destructor
    ~GpuProgramManager();
    /// compile vertex and fragment shader from the given hlsl source code
    GpuProgram* LoadFromSource(const char* const name, const char* const source, const char* const vertexShaderFunctionName, const char* const fragmentShaderFunctionName, const char* const* const macros = NULL);
    /// create, compile and link a gpu program from the given glsl shader source code
    GpuProgram* LoadFromSource(const char* const name, const char* const vertexShaderSource, const char* const fragmentShaderSource);
    /// retrieves a pointer to a gpuprogram by name, or null if the resource does not exist
    GpuProgram* GetByName(const char* const name);
    /// removes all resources
    void RemoveAll();

private:
    /// copy constructor
    GpuProgramManager(const GpuProgramManager&);
    /// prevent copy by assignment
    GpuProgramManager& operator = (const GpuProgramManager&);

    static GpuProgramManager* Singleton;

    ce_hash_table resources;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
