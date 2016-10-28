//------------------------------------------------------------------------------
//  gles2gpuprogram.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "gles2gpuprogram.h"
#include "gles2debug.h"
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
GLES2GpuProgram::GLES2GpuProgram(const char* vertexShaderSource, const char* fragmentShaderSource) :
    defaultParams(NULL)
{
    GLuint vertexShader = this->CreateShaderFromString(GL_VERTEX_SHADER, vertexShaderSource);
    CE_ASSERT(0 != vertexShader, "Could not create vertex shader.");
    GLuint fragmentShader = this->CreateShaderFromString(GL_FRAGMENT_SHADER, fragmentShaderSource);
    CE_ASSERT(0 != fragmentShader, "Could not create fragment shader.");
    this->gpuProgram = this->CreateProgram(vertexShader, fragmentShader);
    CE_ASSERT(0 != this->gpuProgram, "Could not create program.");

    this->uniformLocations[graphics::GpuProgramParameters::ACT_WORLD_MATRIX] = glGetUniformLocation(this->gpuProgram, "worldMatrix");
    CE_GL_ERROR_CHECK("glGetUniformLocation");
    this->uniformLocations[graphics::GpuProgramParameters::ACT_VIEW_MATRIX] = glGetUniformLocation(this->gpuProgram, "viewMatrix");
    CE_GL_ERROR_CHECK("glGetUniformLocation");
    this->uniformLocations[graphics::GpuProgramParameters::ACT_PROJECTION_MATRIX] = glGetUniformLocation(this->gpuProgram, "projectionMatrix");
    CE_GL_ERROR_CHECK("glGetUniformLocation");
    this->uniformLocations[graphics::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX] = glGetUniformLocation(this->gpuProgram, "worldViewProjMatrix");
    CE_GL_ERROR_CHECK("glGetUniformLocation");
    this->uniformLocations[graphics::GpuProgramParameters::ACT_MORPH_WEIGHT] = glGetUniformLocation(this->gpuProgram, "morphWeight");
    CE_GL_ERROR_CHECK("glGetUniformLocation");
    this->uniformLocations[graphics::GpuProgramParameters::ACT_TEXTURE_MATRIX] = -1;

    this->attributeLocations[graphics::VES_POSITION] = glGetAttribLocation(this->gpuProgram, "position");
    CE_GL_ERROR_CHECK("glGetAttribLocation");
    this->attributeLocations[graphics::VES_NORMAL] = glGetAttribLocation(this->gpuProgram, "normal");
    CE_GL_ERROR_CHECK("glGetAttribLocation");
    this->attributeLocations[graphics::VES_TEXTURE_COORDINATES] = glGetAttribLocation(this->gpuProgram, "texCoordIn");
    CE_GL_ERROR_CHECK("glGetAttribLocation");
    this->attributeLocations[graphics::VES_POSITION_MORPH_TARGET] = glGetAttribLocation(this->gpuProgram, "positionMorphTarget");
    CE_GL_ERROR_CHECK("glGetAttribLocation");

    this->constantDefs = CE_NEW graphics::GpuNamedConstants;
    this->ExtractConstantDefs();
}

//------------------------------------------------------------------------------
/**
*/
GLES2GpuProgram::~GLES2GpuProgram()
{
    CE_DELETE this->constantDefs;
    if (this->defaultParams != NULL)
    {
        CE_DELETE this->defaultParams;
    }

    GLint numAttachedShaders;
    glGetProgramiv(this->gpuProgram, GL_ATTACHED_SHADERS, &numAttachedShaders);

    GLsizei shaderCount = 0;
    GLuint* shaders = (GLuint*)CE_MALLOC(numAttachedShaders * sizeof(GLuint));
    glGetAttachedShaders(this->gpuProgram, numAttachedShaders, &shaderCount, shaders);

    GLint i;
    for (i = 0; i < shaderCount; ++i)
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
GLES2GpuProgram::ExtractConstantDefs()
{
    int samplerIndex = 0;

    GLint activeUniformMaxLength = -1;
    glGetProgramiv(this->gpuProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &activeUniformMaxLength);
    GLchar* stringBuffer = (GLchar*)CE_MALLOC(activeUniformMaxLength);

    GLint numActiveUniforms = -1;
    glGetProgramiv(this->gpuProgram, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
    GLint i;
    for (i = 0; i < numActiveUniforms; ++i)
    {
        GLsizei uniformNameLength = -1;
        GLint arraySize = -1;
        GLenum type;
        glGetActiveUniform(this->gpuProgram, i, activeUniformMaxLength, &uniformNameLength, &arraySize, &type, stringBuffer);
        if (graphics::GpuProgramParameters::ACT_COUNT == graphics::GpuProgramParameters::AutoConstantTypeFromString(stringBuffer))
        {
            graphics::GpuConstantDefinition* uniform = CE_NEW graphics::GpuConstantDefinition();
            uniform->location = glGetUniformLocation(this->gpuProgram, stringBuffer);
            switch (type)
            {
                case GL_INT:
                    uniform->constType = graphics::GCT_INT1;
                    uniform->size = sizeof(int);
                    uniform->arraySize = arraySize;
                    uniform->buffer = CE_MALLOC(uniform->size * arraySize);
                    break;
                case GL_FLOAT:
                    uniform->constType = graphics::GCT_FLOAT1;
                    uniform->size = sizeof(float);
                    uniform->arraySize = arraySize;
                    uniform->buffer = CE_MALLOC(uniform->size * arraySize);
                    break;
                case GL_FLOAT_VEC3:
                    uniform->constType = graphics::GCT_FLOAT3;
                    uniform->size = sizeof(float) * 3;
                    uniform->arraySize = arraySize;
                    uniform->buffer = CE_MALLOC(uniform->size * arraySize);
                   break;
                case GL_FLOAT_VEC4:
                    uniform->constType = graphics::GCT_FLOAT4;
                    uniform->size = sizeof(float) * 4;
                    uniform->arraySize = arraySize;
                    uniform->buffer = CE_MALLOC(uniform->size * arraySize);
                   break;
                case GL_FLOAT_MAT4:
                    uniform->constType = graphics::GCT_MATRIX_4X4;
                    uniform->size = sizeof(float) * 16;
                    uniform->arraySize = arraySize;
                    uniform->buffer = CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, uniform->size * arraySize);
                    break;
                case GL_SAMPLER_2D:
                    uniform->constType = graphics::GCT_SAMPLER2D;
                    uniform->size = sizeof(int);
                    uniform->arraySize = arraySize;
                    uniform->buffer = CE_MALLOC(uniform->size * arraySize);
                    memcpy(uniform->buffer, &samplerIndex, uniform->size);
                    ++samplerIndex;
                    break;
                default:
                    CE_ASSERT(false, "GLES2GpuProgram::ExtractConstantDefs(): unsupported uniform type '%i'\n", type);
            }

            CE_LOG("GLES2GpuProgram::ExtractConstantDefs(): add '%i' '%s' '%u' '%i' '%i'\n", type, stringBuffer, uniform->size, uniform->location, arraySize);

            ce_hash_table_insert(&this->constantDefs->map, stringBuffer, uniform);
        }
    }

    CE_FREE(stringBuffer);
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
        CE_GL_ERROR_CHECK("glAttachShader");
        glAttachShader(program, fragmentShader);
        CE_GL_ERROR_CHECK("glAttachShader");
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

} // namespace chrissly
