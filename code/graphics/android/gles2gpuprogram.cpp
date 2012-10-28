//------------------------------------------------------------------------------
//  gles2gpuprogram.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "gles2gpuprogram.h"
#include "gles2rendersystem.h"
#include "debug.h"

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
GLES2GpuProgram::GLES2GpuProgram()
{

}

//------------------------------------------------------------------------------
/**
*/
GLES2GpuProgram::GLES2GpuProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    GLuint vertexShader = this->CreateShaderFromString(GL_VERTEX_SHADER, vertexShaderSource);
    CE_ASSERT(0 != vertexShader, "Could not create vertex shader.");
    GLuint fragmentShader = this->CreateShaderFromString(GL_FRAGMENT_SHADER, fragmentShaderSource);
    CE_ASSERT(0 != fragmentShader, "Could not create fragment shader.");
    this->gpuProgram = this->CreateProgram(vertexShader, fragmentShader);
    CE_ASSERT(0 != this->gpuProgram, "Could not create program.");

    this->textureHandle = glGetUniformLocation(this->gpuProgram, "texture");
    GLES2RenderSystem::CheckGlError("glGetUniformLocation");
    this->uniformLocations[graphics::ACT_WORLD_MATRIX] = glGetUniformLocation(this->gpuProgram, "worldMatrix");
    GLES2RenderSystem::CheckGlError("glGetUniformLocation");
    this->uniformLocations[graphics::ACT_VIEW_MATRIX] = glGetUniformLocation(this->gpuProgram, "viewMatrix");
    GLES2RenderSystem::CheckGlError("glGetUniformLocation");
    this->uniformLocations[graphics::ACT_PROJECTION_MATRIX] = glGetUniformLocation(this->gpuProgram, "projectionMatrix");
    GLES2RenderSystem::CheckGlError("glGetUniformLocation");
    this->uniformLocations[graphics::ACT_WORLDVIEWPROJ_MATRIX] = glGetUniformLocation(this->gpuProgram, "worldViewProjMatrix");
    GLES2RenderSystem::CheckGlError("glGetUniformLocation");
    this->uniformLocations[graphics::ACT_TEXTURE_MATRIX] = 0;

    this->attributeLocations[graphics::VES_POSITION] = glGetAttribLocation(this->gpuProgram, "vertexPosition");
    GLES2RenderSystem::CheckGlError("glGetAttribLocation");
    this->attributeLocations[graphics::VES_TEXTURE_COORDINATES] = glGetAttribLocation(this->gpuProgram, "texCoordIn");
    GLES2RenderSystem::CheckGlError("glGetAttribLocation");
}

//------------------------------------------------------------------------------
/**
*/
GLES2GpuProgram::~GLES2GpuProgram()
{
    GLint numAttachedShaders;
    glGetProgramiv(this->gpuProgram, GL_ATTACHED_SHADERS, &numAttachedShaders);

    GLsizei shaderCount = 0;
    GLuint* shaders = (GLuint*) CE_MALLOC(numAttachedShaders * sizeof(GLuint));
    glGetAttachedShaders(this->gpuProgram, numAttachedShaders, &shaderCount, shaders);

    GLint i;
    for (i = 0; i < shaderCount; i++)
    {
        glDetachShader(this->gpuProgram, shaders[i]);
        glDeleteShader(shaders[i]);
    }

    CE_FREE(shaders);

    glDeleteProgram(this->gpuProgram);
}

//------------------------------------------------------------------------------
/**
*/
GLint
GLES2GpuProgram::GetUniformLocation(graphics::AutoConstantType acType) const
{
    return this->uniformLocations[acType];
}

//------------------------------------------------------------------------------
/**
*/
GLint
GLES2GpuProgram::GetTextureUniformLocation() const
{
    return this->textureHandle;
}

//------------------------------------------------------------------------------
/**
*/
GLint
GLES2GpuProgram::GetProgramHandle() const
{
    return this->gpuProgram;
}

//------------------------------------------------------------------------------
/**
*/
GLint
GLES2GpuProgram::GetAttributeLocation(graphics::VertexElementSemantic semantic)
{
    return this->attributeLocations[semantic];
}

//------------------------------------------------------------------------------
/**
*/
GLuint
GLES2GpuProgram::CreateShaderFromString(GLenum shaderType, const char* source)
{
    GLuint shader = glCreateShader(shaderType);
    if (shader != 0)
    {
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (0 == compiled)
        {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 0)
            {
                char* buf = (char*) CE_MALLOC(infoLen);
                if (buf != NULL)
                {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    CE_ASSERT(false, "Could not compile shader %d: %s\n", shaderType, buf);
                    CE_FREE(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }

    return shader;
}

//------------------------------------------------------------------------------
/**
*/
GLuint
GLES2GpuProgram::CreateProgram(GLuint vertexShader, GLuint fragmentShader)
{
    if (0 == vertexShader || 0 == fragmentShader)
    {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program != 0)
    {
        glAttachShader(program, vertexShader);
        GLES2RenderSystem::CheckGlError("glAttachShader");
        glAttachShader(program, fragmentShader);
        GLES2RenderSystem::CheckGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE)
        {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength > 0)
            {
                char* buf = (char*) CE_MALLOC(bufLength);
                if (buf != NULL)
                {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    CE_ASSERT(false, "Could not link program: %s\n", buf);
                    CE_FREE(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }

    return program;
}
    
} // namespace chrissly
