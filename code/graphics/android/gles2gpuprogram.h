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
    /// returns the location of a uniform variable
    GLint GetUniformLocation(graphics::AutoConstantType acType) const;
    /// returns the location of the uniform sampler2D variable
    GLint GetTextureUniformLocation() const;
    /// get the handle for the program object
    GLint GetProgramHandle() const;
    /// get the index of a non-standard attribute bound in the linked code
    GLint GetAttributeLocation(graphics::VertexElementSemantic semantic);

private:
    /// private default constructor
    GLES2GpuProgram();
    /// create and compiles a shader from the given string and returns a handle to it
    GLuint CreateShaderFromString(GLenum shaderType, const char* source);
    /// creates a gpu program from the given shader handles
    GLuint CreateProgram(GLuint vertexShader, GLuint fragmentShader);
    
    GLuint gpuProgram;
    GLint textureHandle;
    GLint uniformLocations[graphics::ACT_COUNT];
    GLint attributeLocations[graphics::VES_COUNT];
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
