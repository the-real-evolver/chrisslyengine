#ifndef GLES2GPUPROGRAM_H_
#define GLES2GPUPROGRAM_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::GLES2GpuProgram

    (C) 2012 Christian Bleicher
*/
#include "hardwarevertexbuffer.h"
#include "gpuprogramparams.h"
#include <GLES2/gl2.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class GLES2GpuProgram
{
public:
    /// construct from vertex and fragment source
    GLES2GpuProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
    /// destructor
    ~GLES2GpuProgram();
    /// get a reference to the default parameters which are to be used for all uses of this program
    graphics::GpuProgramParameters* GetDefaultParameters();
    /// get the full list of named constants
    graphics::GpuNamedConstants* GetConstantDefinitions() const;
    /// returns the location of a uniform variable
    GLint GetUniformLocation(graphics::GpuProgramParameters::AutoConstantType acType) const;
    /// get the handle for the program object
    GLint GetProgramHandle() const;
    /// get the index of a non-standard attribute bound in the linked code
    GLint GetAttributeLocation(graphics::VertexElementSemantic semantic) const;

private:
    /// private default constructor
    GLES2GpuProgram();
    /// creates a new parameters object compatible with this program definition
    graphics::GpuProgramParameters* CreateParameters();
    /// populate named constants
    void ExtractConstantDefs();
    /// create and compiles a shader from the given string and returns a handle to it
    GLuint CreateShaderFromString(GLenum shaderType, const char* source);
    /// creates a gpu program from the given shader handles
    GLuint CreateProgram(GLuint vertexShader, GLuint fragmentShader);

    graphics::GpuProgramParameters* defaultParams;
    graphics::GpuNamedConstants* constantDefs;
    GLuint gpuProgram;
    GLint uniformLocations[graphics::GpuProgramParameters::ACT_COUNT];
    GLint attributeLocations[graphics::VES_COUNT];
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
