//------------------------------------------------------------------------------
//  gles2gpuprogram.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "gles2gpuprogram.h"
#include "gles2rendersystem.h"
#include "chrisslystring.h"
#include "debug.h"

namespace chrissly
{

// valid white spaces in GLSL: the space character, horizontal tab, vertical tab, form feed, carriage-return, and line-feed
#define CE_GLSL_FIND_NEXT_TOKEN(str, start, end) \
    while (' ' == str[0] || '\t' == str[0]  || '\v' == str[0] || '\f' == str[0] || '\r' == str[0] || '\n' == str[0]) {str++;} \
    start = str; \
    while (' ' != str[0] && '\t' != str[0]  && '\v' != str[0] && '\f' != str[0] && '\r' != str[0] && '\n' != str[0] && str[0] != ';') {str++;} \
    end = str;

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

    this->uniformLocations[graphics::GpuProgramParameters::ACT_WORLD_MATRIX] = glGetUniformLocation(this->gpuProgram, "worldMatrix");
    GLES2RenderSystem::CheckGlError("glGetUniformLocation");
    this->uniformLocations[graphics::GpuProgramParameters::ACT_VIEW_MATRIX] = glGetUniformLocation(this->gpuProgram, "viewMatrix");
    GLES2RenderSystem::CheckGlError("glGetUniformLocation");
    this->uniformLocations[graphics::GpuProgramParameters::ACT_PROJECTION_MATRIX] = glGetUniformLocation(this->gpuProgram, "projectionMatrix");
    GLES2RenderSystem::CheckGlError("glGetUniformLocation");
    this->uniformLocations[graphics::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX] = glGetUniformLocation(this->gpuProgram, "worldViewProjMatrix");
    GLES2RenderSystem::CheckGlError("glGetUniformLocation");
    this->uniformLocations[graphics::GpuProgramParameters::ACT_MORPH_WEIGHT] = glGetUniformLocation(this->gpuProgram, "morphWeight");
    GLES2RenderSystem::CheckGlError("glGetUniformLocation");
    this->uniformLocations[graphics::GpuProgramParameters::ACT_TEXTURE_MATRIX] = -1;

    this->attributeLocations[graphics::VES_POSITION] = glGetAttribLocation(this->gpuProgram, "position");
    GLES2RenderSystem::CheckGlError("glGetAttribLocation");
    this->attributeLocations[graphics::VES_NORMAL] = glGetAttribLocation(this->gpuProgram, "normal");
    GLES2RenderSystem::CheckGlError("glGetAttribLocation");
    this->attributeLocations[graphics::VES_TEXTURE_COORDINATES] = glGetAttribLocation(this->gpuProgram, "texCoordIn");
    GLES2RenderSystem::CheckGlError("glGetAttribLocation");
    this->attributeLocations[graphics::VES_POSITION_MORPH_TARGET] = glGetAttribLocation(this->gpuProgram, "positionMorphTarget");
    GLES2RenderSystem::CheckGlError("glGetAttribLocation");

    this->constantDefs = CE_NEW graphics::GpuNamedConstants;
    this->ExtractConstantDefs(vertexShaderSource, this->constantDefs);
    this->ExtractConstantDefs(fragmentShaderSource, this->constantDefs);

    this->defaultParams = NULL;
}

//------------------------------------------------------------------------------
/**
*/
GLES2GpuProgram::~GLES2GpuProgram()
{
    CE_DELETE this->constantDefs;
    if (this->defaultParams != NULL) CE_DELETE this->defaultParams;

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
graphics::GpuProgramParameters*
GLES2GpuProgram::GetDefaultParameters()
{
    if (NULL == this->defaultParams)
    {
        this->defaultParams = this->CreateParameters();
    }

    return this->defaultParams;
}

//------------------------------------------------------------------------------
/**
*/
graphics::GpuNamedConstants*
GLES2GpuProgram::GetConstantDefinitions() const
{
    return this->constantDefs;
}

//------------------------------------------------------------------------------
/**
*/
GLint
GLES2GpuProgram::GetUniformLocation(graphics::GpuProgramParameters::AutoConstantType acType) const
{
    return this->uniformLocations[acType];
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
GLES2GpuProgram::GetAttributeLocation(graphics::VertexElementSemantic semantic) const
{
    return this->attributeLocations[semantic];
}

//------------------------------------------------------------------------------
/**
*/
graphics::GpuProgramParameters*
GLES2GpuProgram::CreateParameters()
{
    graphics::GpuProgramParameters* ret = CE_NEW graphics::GpuProgramParameters;

    if (this->constantDefs != NULL)
    {
        ret->_SetNamedConstants(this->constantDefs);
    }

    return ret;
}

//------------------------------------------------------------------------------
/**
*/
void
GLES2GpuProgram::ExtractConstantDefs(const char* source, graphics::GpuNamedConstants* constantDefs)
{
    char *start, *end;
    char* match = strstr(source, "uniform");
    int samplerIndex = 0;
    while (match != NULL)
    {
        // skip 'uniform'
        match += 7;

        // parse datatype
        CE_GLSL_FIND_NEXT_TOKEN(match, start, end);
        core::String dataType;
        dataType.Set(start, end - start);
        if (0 == strcmp(dataType.C_Str(), "lowp") || 0 == strcmp(dataType.C_Str(), "mediump") || 0 == strcmp(dataType.C_Str(), "highp"))
        {
            // skip precision if specified
            CE_GLSL_FIND_NEXT_TOKEN(match, start, end);
            dataType.Set(start, end - start);
        }
        graphics::GpuConstantType constantType;
        if (0 == strcmp(dataType.C_Str(), "mat4"))
        {
            constantType = graphics::GCT_MATRIX_4X4;
        }
        else if (0 == strcmp(dataType.C_Str(), "float"))
        {
            constantType = graphics::GCT_FLOAT1;
        }
        else if (0 == strcmp(dataType.C_Str(), "sampler2D"))
        {
            constantType = graphics::GCT_SAMPLER2D;
        }
        else if (0 == strcmp(dataType.C_Str(), "int"))
        {
            constantType = graphics::GCT_INT1;
        }
        else
        {
            match = strstr(match, "uniform");
            continue;
        }

        // parse uniform name
        CE_GLSL_FIND_NEXT_TOKEN(match, start, end);
        core::String uniformName;
        uniformName.Set(start, end - start);

        if (!this->IsAutoConstantType(uniformName.C_Str()))
        {
            GLint uniformLocation = glGetUniformLocation(this->gpuProgram, uniformName.C_Str());
            if (uniformLocation != -1)
            {
                graphics::GpuConstantDefinition* uniform = CE_NEW graphics::GpuConstantDefinition();
                uniform->location = uniformLocation;
                uniform->constType = constantType;
                switch (constantType)
                {
                    case graphics::GCT_MATRIX_4X4:
                        uniform->size = sizeof(float) * 16;
                        uniform->buffer = CE_MALLOC(uniform->size);
                        break;
                    case graphics::GCT_FLOAT1:
                        uniform->size = sizeof(float);
                        uniform->buffer = CE_MALLOC(uniform->size);
                        break;
                    case graphics::GCT_INT1:
                        uniform->size = sizeof(int);
                        uniform->buffer = CE_MALLOC(uniform->size);
                        break;
                    case graphics::GCT_SAMPLER2D:
                        uniform->size = sizeof(int);
                        uniform->buffer = CE_MALLOC(uniform->size);
                        memcpy(uniform->buffer, &samplerIndex, uniform->size);
                        samplerIndex++;
                        break;
                }

                CE_LOG("GLES2GpuProgram::ExtractConstantDefs: add '%s' '%s' '%u' '%i'\n", dataType.C_Str(), uniformName.C_Str(), uniform->size, uniform->location);

                HashTableInsert(&constantDefs->map, uniformName.C_Str(), uniform);
            }
        }

        match = strstr(match, "uniform");
    }
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
                char* buf = (char*)CE_MALLOC(infoLen);
                if (buf != NULL)
                {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    CE_LOG("Could not compile shader %d: %s\n", shaderType, buf);
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
                char* buf = (char*)CE_MALLOC(bufLength);
                if (buf != NULL)
                {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    CE_LOG("Could not link program: %s\n", buf);
                    CE_FREE(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }

    return program;
}

//------------------------------------------------------------------------------
/**
*/
bool
GLES2GpuProgram::IsAutoConstantType(const char* name)
{
    if (0 == strcmp(name, "worldMatrix"))         return true;
    if (0 == strcmp(name, "viewMatrix"))          return true;
    if (0 == strcmp(name, "projectionMatrix"))    return true;
    if (0 == strcmp(name, "worldViewProjMatrix")) return true;
    if (0 == strcmp(name, "morphWeight"))         return true;

    return false;
}

} // namespace chrissly
