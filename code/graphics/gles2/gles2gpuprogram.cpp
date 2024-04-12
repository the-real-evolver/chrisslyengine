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
GLES2GpuProgram::GLES2GpuProgram(const char* const vertexShaderSource, const char* const fragmentShaderSource, const char* const* const macros) :
    defaultParams(NULL)
{
    GLuint vertexShader = CreateShaderFromString(GL_VERTEX_SHADER, vertexShaderSource, macros);
    CE_ASSERT(0U != vertexShader, "GLES2GpuProgram::GLES2GpuProgram(): Could not create vertex shader.\n");
    GLuint fragmentShader = CreateShaderFromString(GL_FRAGMENT_SHADER, fragmentShaderSource, macros);
    CE_ASSERT(0U != fragmentShader, "GLES2GpuProgram::GLES2GpuProgram(): Could not create fragment shader.\n");
    this->gpuProgram = CreateProgram(vertexShader, fragmentShader);
    CE_ASSERT(0U != this->gpuProgram, "GLES2GpuProgram::GLES2GpuProgram(): Could not create program.\n");

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
    this->uniformLocations[graphics::GpuProgramParameters::ACT_TEXTURE_MATRIX] = glGetUniformLocation(this->gpuProgram, "textureMatrix");
    CE_GL_ERROR_CHECK("glGetUniformLocation");
    this->uniformLocations[graphics::GpuProgramParameters::ACT_BONE_MATRICES] = glGetUniformLocation(this->gpuProgram, "boneMatrices[0]");
    CE_GL_ERROR_CHECK("glGetUniformLocation");

    this->attributeLocations[graphics::VES_POSITION] = glGetAttribLocation(this->gpuProgram, "position");
    CE_GL_ERROR_CHECK("glGetAttribLocation");
    this->attributeLocations[graphics::VES_BLEND_WEIGHTS] = glGetAttribLocation(this->gpuProgram, "weights");
    CE_GL_ERROR_CHECK("glGetAttribLocation");
    this->attributeLocations[graphics::VES_BLEND_INDICES] = glGetAttribLocation(this->gpuProgram, "indices");
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
GLuint
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
                    uniform->size = sizeof(float) * 3U;
                    uniform->arraySize = arraySize;
                    uniform->buffer = CE_MALLOC(uniform->size * arraySize);
                   break;
                case GL_FLOAT_VEC4:
                    uniform->constType = graphics::GCT_FLOAT4;
                    uniform->size = sizeof(float) * 4U;
                    uniform->arraySize = arraySize;
                    uniform->buffer = CE_MALLOC(uniform->size * arraySize);
                   break;
                case GL_FLOAT_MAT4:
                    uniform->constType = graphics::GCT_MATRIX_4X4;
                    uniform->size = sizeof(float) * 16U;
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
                case GL_SAMPLER_CUBE:
                    uniform->constType = graphics::GCT_SAMPLERCUBE;
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

            ce_hash_table_insert(&this->constantDefs->map, stringBuffer, strlen(stringBuffer), uniform);
        }
    }

    CE_FREE(stringBuffer);
}

//------------------------------------------------------------------------------
/**
*/
GLuint
GLES2GpuProgram::CreateShaderFromString(GLenum shaderType, const char* source, const char* const* const macros)
{
    char* srcMod = (char*)source;
    if (macros != NULL)
    {
        unsigned int versionLen = 0U;
        const char* versionBegin = strstr(source, "#version");
        const char* versionEnd = NULL;
        if (versionBegin != NULL)
        {
            versionEnd = strchr(versionBegin, '\n');
            versionLen = (versionEnd - versionBegin) + 1U;
        }

        unsigned int definesLen = 0U, index = 0U;
        while (macros[index] != NULL)
        {
            definesLen += strlen("#define  \n") + strlen(macros[index]) + strlen(macros[index + 1U]);
            index += 2U;
        }

        unsigned int sourceLen = strlen(source);
        srcMod = (char*)CE_MALLOC(sourceLen + definesLen);
        memset(srcMod, 0, sourceLen + definesLen);
        if (versionLen > 0U) strncpy(srcMod, versionBegin, versionLen);
        index = 0U;
        while (macros[index] != NULL)
        {
            strcat(srcMod, "#define ");
            strcat(srcMod, macros[index]);
            strcat(srcMod, " ");
            strcat(srcMod, macros[index + 1U]);
            strcat(srcMod, "\n");
            index += 2U;
        }
        strcat(srcMod, versionLen > 0U ? versionEnd + 1U : source);
    }

    GLuint shader = glCreateShader(shaderType);
    if (shader != 0U)
    {
        glShaderSource(shader, 1, &srcMod, NULL);
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
                shader = 0U;
            }
        }
    }

    if (macros != NULL) CE_FREE(srcMod);

    return shader;
}

//------------------------------------------------------------------------------
/**
*/
GLuint
GLES2GpuProgram::CreateProgram(GLuint vertexShader, GLuint fragmentShader)
{
    if (0U == vertexShader || 0U == fragmentShader)
    {
        return 0U;
    }

    GLuint program = glCreateProgram();
    if (program != 0U)
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
            program = 0U;
        }
    }

    return program;
}

} // namespace chrissly
