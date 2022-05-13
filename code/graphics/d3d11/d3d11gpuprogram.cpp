//------------------------------------------------------------------------------
//  d3d11gpuprogram.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "d3d11gpuprogram.h"
#include "d3d11constantbuffer.h"
#include "d3d11rendersystem.h"
#include "d3d11mappings.h"
#include "chrisslyarray.h"
#include "debug.h"
#include <D3DCompiler.h>

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
D3D11GpuProgram::D3D11GpuProgram(const char* const source, const char* const fileName, const char* const vertexShaderFunctionName, const char* const fragmentShaderFunctionName, const char* const* const macros) :
    defaultParams(NULL),
    constantDefs(NULL),
    constantBuffersPerObject(NULL),
    constantBuffersPerPass(NULL),
    vertexShaderCode(NULL),
    vertexShader(NULL),
    fragmentShader(NULL),
    bufferSlot(0U)
{
    /* compile and create vertex shader */
    ID3D10Blob* errorBlob = NULL;
    HRESULT result = D3DCompile(
        source,                             /* _In_reads_bytes_(SrcDataSize)                            LPCVOID pSrcData                    */
        strlen(source),                     /* _In_                                                     SIZE_T SrcDataSize                  */
        fileName,                           /* _In_opt_                                                 LPCSTR pSourceName                  */
        (D3D_SHADER_MACRO*)macros,          /* _In_reads_opt_(_Inexpressible_(pDefines->Name != NULL))  CONST D3D_SHADER_MACRO* pDefines    */
        NULL,                               /* _In_opt_                                                 ID3DInclude* pInclude               */
        vertexShaderFunctionName,           /* _In_opt_                                                 LPCSTR pEntrypoint                  */
        "vs_4_0",                           /* _In_                                                     LPCSTR pTarget                      */
        D3DCOMPILE_WARNINGS_ARE_ERRORS,     /* _In_                                                     UINT Flags1                         */
        0U,                                 /* _In_                                                     UINT Flags2                         */
        &this->vertexShaderCode,            /* _Out_                                                    ID3DBlob** ppCode                   */
        &errorBlob                          /* _Out_opt_                                                ID3DBlob** ppErrorMsgs              */
    );

    if (FAILED(result))
    {
        if (errorBlob != NULL)
        {
            char* errorMsg = (char*)errorBlob->GetBufferPointer();
            errorMsg[errorBlob->GetBufferSize() - 1U] = '\0';
            CE_ASSERT(SUCCEEDED(result), "\nD3D11GpuProgram::D3D11GpuProgram(): failed to compile vertex shader '%s' in file '%s'\nError: '%s'\n", vertexShaderFunctionName, fileName, errorMsg);
            errorBlob->Release();
            errorBlob = NULL;
        }
    }

    result = D3D11RenderSystem::Instance()->GetDevice()->CreateVertexShader(
        this->vertexShaderCode->GetBufferPointer(),
        this->vertexShaderCode->GetBufferSize(),
        NULL,
        &this->vertexShader
    );
    CE_ASSERT(SUCCEEDED(result), "D3D11GpuProgram::D3D11GpuProgram(): failed to create vertex shader '%s' in file '%s'\n", vertexShaderFunctionName, fileName);

    /* compile and create fragment shader */
    ID3D10Blob* fragmentShaderCode = NULL;
    result = D3DCompile(
        source,                             /* _In_reads_bytes_(SrcDataSize)                            LPCVOID pSrcData                    */
        strlen(source),                     /* _In_                                                     SIZE_T SrcDataSize                  */
        fileName,                           /* _In_opt_                                                 LPCSTR pSourceName                  */
        (D3D_SHADER_MACRO*)macros,          /* _In_reads_opt_(_Inexpressible_(pDefines->Name != NULL))  CONST D3D_SHADER_MACRO* pDefines    */
        NULL,                               /* _In_opt_                                                 ID3DInclude* pInclude               */
        fragmentShaderFunctionName,         /* _In_opt_                                                 LPCSTR pEntrypoint                  */
        "ps_4_0",                           /* _In_                                                     LPCSTR pTarget                      */
        D3DCOMPILE_WARNINGS_ARE_ERRORS,     /* _In_                                                     UINT Flags1                         */
        0U,                                 /* _In_                                                     UINT Flags2                         */
        &fragmentShaderCode,                /* _Out_                                                    ID3DBlob** ppCode                   */
        &errorBlob                          /* _Out_opt_                                                ID3DBlob** ppErrorMsgs              */
    );

    if (FAILED(result))
    {
        if (errorBlob != NULL)
        {
            char* errorMsg = (char*)errorBlob->GetBufferPointer();
            errorMsg[errorBlob->GetBufferSize() - 1U] = '\0';
            CE_ASSERT(SUCCEEDED(result), "\nD3D11GpuProgram::D3D11GpuProgram(): failed to compile fragment shader '%s' in file '%s'\nError: %s\n", fragmentShaderFunctionName, fileName, errorMsg);
            errorBlob->Release();
            errorBlob = NULL;
        }
    }

    result = D3D11RenderSystem::Instance()->GetDevice()->CreatePixelShader(
        fragmentShaderCode->GetBufferPointer(),
        fragmentShaderCode->GetBufferSize(),
        NULL,
        &this->fragmentShader
    );
    CE_ASSERT(SUCCEEDED(result), "D3D11GpuProgram::D3D11GpuProgram(): failed to create fragment shader '%s' in file '%s'\n", fragmentShaderFunctionName, fileName);

    this->constantDefs = CE_NEW graphics::GpuNamedConstants;
    this->defaultParams = CE_NEW graphics::GpuProgramParameters;
    this->defaultParams->_SetNamedConstants(this->constantDefs);

    ce_array_init(this->constantBuffersPerObject, 1U);
    ce_array_init(this->constantBuffersPerPass, 1U);

    ce_hash_table_init(&this->constantBuffers, 2U);
    this->ExtractConstantDefs(this->vertexShaderCode);
    this->ExtractConstantDefs(fragmentShaderCode);
    ce_hash_table_clear(&this->constantBuffers);

    if (fragmentShaderCode != NULL)
    {
        fragmentShaderCode->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
D3D11GpuProgram::~D3D11GpuProgram()
{
    CE_DELETE this->constantDefs;
    CE_DELETE this->defaultParams;

    unsigned int i;
    for (i = 0U; i < ce_array_size(this->constantBuffersPerObject); ++i)
    {
        CE_DELETE this->constantBuffersPerObject[i];
    }
    ce_array_delete(this->constantBuffersPerObject);

    for (i = 0U; i < ce_array_size(this->constantBuffersPerPass); ++i)
    {
        CE_DELETE this->constantBuffersPerPass[i];
    }
    ce_array_delete(this->constantBuffersPerPass);

    if (this->vertexShaderCode != NULL)
    {
        this->vertexShaderCode->Release();
    }
    if (this->vertexShader != NULL)
    {
        this->vertexShader->Release();
    }
    if (this->fragmentShader != NULL)
    {
        this->fragmentShader->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
graphics::GpuProgramParameters* const
D3D11GpuProgram::GetDefaultParameters() const
{
    return this->defaultParams;
}

//------------------------------------------------------------------------------
/**
*/
graphics::GpuNamedConstants* const
D3D11GpuProgram::GetConstantDefinitions() const
{
    return this->constantDefs;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11GpuProgram::Bind()
{
    ID3D11DeviceContext* context = D3D11RenderSystem::Instance()->GetContext();
    context->VSSetShader(this->vertexShader, NULL, 0U);
    context->PSSetShader(this->fragmentShader, NULL, 0U);
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11GpuProgram::BindConstantBuffers()
{
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->constantBuffersPerObject); ++i)
    {
        this->constantBuffersPerObject[i]->Bind();
    }
    for (i = 0U; i < ce_array_size(this->constantBuffersPerPass); ++i)
    {
        this->constantBuffersPerPass[i]->Bind();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11GpuProgram::UpdatePerObjectConstantBuffers()
{
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->constantBuffersPerObject); ++i)
    {
        this->constantBuffersPerObject[i]->UpdateConstants();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11GpuProgram::UpdatePerPassConstantBuffers()
{
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->constantBuffersPerPass); ++i)
    {
        this->constantBuffersPerPass[i]->UpdateConstants();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11GpuProgram::ExtractConstantDefs(ID3D10Blob* const shaderCode)
{
    ID3D11ShaderReflection* reflector = NULL;
    HRESULT result = D3DReflect(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflector);
    CE_ASSERT(SUCCEEDED(result), "D3D11GpuProgram::ExtractConstantDefs(): failed to reflect vertex shader source code\n");
    D3D11_SHADER_DESC shaderDesc;
    result = reflector->GetDesc(&shaderDesc);
    CE_ASSERT(SUCCEEDED(result), "D3D11GpuProgram::ExtractConstantDefs(): failed to get shader description\n");

    UINT bufferIndex;
    for (bufferIndex = 0U; bufferIndex < shaderDesc.ConstantBuffers; ++bufferIndex)
    {
        bool updatePerObject = false;

        ID3D11ShaderReflectionConstantBuffer* reflectionBuffer = reflector->GetConstantBufferByIndex(bufferIndex);
        CE_ASSERT(reflectionBuffer != NULL, "D3D11GpuProgram::ExtractConstantDefs(): failed to get constant buffer by index\n");
        D3D11_SHADER_BUFFER_DESC bufferDesc;
        result = reflectionBuffer->GetDesc(&bufferDesc);
        CE_ASSERT(SUCCEEDED(result), "D3D11GpuProgram::ExtractConstantDefs(): failed to get buffer description\n");

        /* create constant buffer */
        if (ce_hash_table_find(&this->constantBuffers, bufferDesc.Name, strlen(bufferDesc.Name)) != NULL)
        {
            continue;
        }
        D3D11ConstantBuffer* constantBuffer = CE_NEW D3D11ConstantBuffer(bufferDesc.Size, this->bufferSlot++, bufferDesc.Variables);
        ce_hash_table_insert(&this->constantBuffers, bufferDesc.Name, strlen(bufferDesc.Name), constantBuffer);

        UINT constantIndex;
        for (constantIndex = 0U; constantIndex < bufferDesc.Variables; ++constantIndex)
        {
            ID3D11ShaderReflectionVariable* reflectionVariable = reflectionBuffer->GetVariableByIndex(constantIndex);
            CE_ASSERT(reflectionVariable != NULL, "D3D11GpuProgram::ExtractConstantDefs(): failed to get variable by index\n");
            D3D11_SHADER_VARIABLE_DESC variableDesc;
            result = reflectionVariable->GetDesc(&variableDesc);
            CE_ASSERT(SUCCEEDED(result), "D3D11GpuProgram::ExtractConstantDefs(): failed to get variable description\n");

            ID3D11ShaderReflectionType* reflectionType = reflectionVariable->GetType();
            CE_ASSERT(reflectionType != NULL, "D3D11GpuProgram::ExtractConstantDefs(): failed to get reflection type\n");
            D3D11_SHADER_TYPE_DESC typeDesc;
            result = reflectionType->GetDesc(&typeDesc);
            CE_ASSERT(SUCCEEDED(result), "D3D11GpuProgram::ExtractConstantDefs(): failed to get type description\n");

            /* create constant definiton */
            graphics::GpuConstantDefinition* variable = CE_NEW graphics::GpuConstantDefinition();
            variable->constType = D3D11Mappings::Get(typeDesc);
            variable->location = variableDesc.StartOffset;
            variable->buffer = CE_MALLOC(variableDesc.Size);
            if (typeDesc.Elements > 0U)
            {
                variable->size = variableDesc.Size / typeDesc.Elements;
                variable->arraySize = typeDesc.Elements;
            }
            else
            {
                variable->size = variableDesc.Size;
                variable->arraySize = 1U;
            }

            ce_array_push_back(constantBuffer->constants, variable);

            ce_hash_table_insert(&this->constantDefs->map, variableDesc.Name, strlen(variableDesc.Name), variable);

            switch (graphics::GpuProgramParameters::AutoConstantTypeFromString(variableDesc.Name))
            {
                case graphics::GpuProgramParameters::ACT_WORLD_MATRIX:
                    this->defaultParams->_SetAutoConstant(graphics::GpuProgramParameters::ACT_WORLD_MATRIX, variable);
                    updatePerObject = true;
                    break;
                case graphics::GpuProgramParameters::ACT_VIEW_MATRIX:
                    this->defaultParams->_SetAutoConstant(graphics::GpuProgramParameters::ACT_VIEW_MATRIX, variable);
                    break;
                case graphics::GpuProgramParameters::ACT_PROJECTION_MATRIX:
                    this->defaultParams->_SetAutoConstant(graphics::GpuProgramParameters::ACT_PROJECTION_MATRIX, variable);
                    break;
                case graphics::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX:
                    this->defaultParams->_SetAutoConstant(graphics::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX, variable);
                    updatePerObject = true;
                    break;
                case graphics::GpuProgramParameters::ACT_TEXTURE_MATRIX:
                    this->defaultParams->_SetAutoConstant(graphics::GpuProgramParameters::ACT_TEXTURE_MATRIX, variable);
                    updatePerObject = true;
                    break;
                case graphics::GpuProgramParameters::ACT_MORPH_WEIGHT:
                    this->defaultParams->_SetAutoConstant(graphics::GpuProgramParameters::ACT_MORPH_WEIGHT, variable);
                    updatePerObject = true;
                    break;
            }

            CE_LOG("D3D11GpuProgram::ExtractConstantDefs(): add type: %i name: '%s' size: %u offset: %i arraySize %i\n", variable->constType, variableDesc.Name, variable->size, variable->location, variable->arraySize);
        }

        ce_array_push_back(updatePerObject ? this->constantBuffersPerObject : this->constantBuffersPerPass, constantBuffer);
    }
}

//------------------------------------------------------------------------------
/**
*/
ID3D10Blob* const
D3D11GpuProgram::GetVertexShaderCode() const
{
    return this->vertexShaderCode;
}

} // namespace chrissly
