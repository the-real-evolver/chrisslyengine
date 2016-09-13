//------------------------------------------------------------------------------
//  d3d11gpuprogram.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "d3d11gpuprogram.h"
#include "d3d11rendersystem.h"
#include "debug.h"

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
D3D11GpuProgram::D3D11GpuProgram()
{

}

//------------------------------------------------------------------------------
/**
*/
D3D11GpuProgram::D3D11GpuProgram(const char* source, const char* fileName, const char* vertexShaderFunctionName, const char* fragmentShaderFunctionName) :
    vertexShaderCode(NULL),
    fragmentShaderCode(NULL),
    vertexShader(NULL),
    fragmentShader(NULL),
    autoConstantBuffer(NULL)
{
    /* compile and create vertex shader */
    ID3D10Blob* errorBlob = NULL;
    HRESULT result = D3DX11CompileFromMemory(
        source,                             /* __in     LPCSTR pSrcData                     */
        strlen(source),                     /* __in     SIZE_T SrcDataLen                   */
        fileName,                           /* __in     LPCSTR pFileName                    */
        NULL,                               /* __in     const D3D10_SHADER_MACRO *pDefines  */
        NULL,                               /* __in     LPD3D10INCLUDE pInclude             */
        vertexShaderFunctionName,           /* __in     LPCSTR pFunctionName                */
        "vs_4_0",                           /* __in     LPCSTR pProfile                     */
        D3D10_SHADER_OPTIMIZATION_LEVEL2,   /* __in     UINT Flags1                         */
        0,                                  /* __in     UINT Flags2                         */
        NULL,                               /* __in     ID3DX11ThreadPump *pPump            */
        &this->vertexShaderCode,            /* __out    ID3D10Blob **ppShader               */
        &errorBlob,                         /* __out    ID3D10Blob **ppErrorMsgs            */
        NULL                                /* __out    HRESULT *pHResult                   */
    );
    CE_ASSERT(SUCCEEDED(result), "D3D11GpuProgram::D3D11GpuProgram(): failed to compile vertex shader '%s' in file '%s'\n", vertexShaderFunctionName, fileName);

    result = D3D11RenderSystem::Instance()->GetDevice()->CreateVertexShader(
        this->vertexShaderCode->GetBufferPointer(),
        this->vertexShaderCode->GetBufferSize(),
        NULL,
        &this->vertexShader
    );
    CE_ASSERT(SUCCEEDED(result), "D3D11GpuProgram::D3D11GpuProgram(): failed to create vertex shader '%s' in file '%s'\n", vertexShaderFunctionName, fileName);

    if (errorBlob != NULL)
    {
        errorBlob->Release();
        errorBlob = NULL;
    }

    /* compile and create fragment shader */
    result = D3DX11CompileFromMemory(
        source,                             /* __in     LPCSTR pSrcData                     */
        strlen(source),                     /* __in     SIZE_T SrcDataLen                   */
        fileName,                           /* __in     LPCSTR pFileName                    */
        NULL,                               /* __in     const D3D10_SHADER_MACRO *pDefines  */
        NULL,                               /* __in     LPD3D10INCLUDE pInclude             */
        fragmentShaderFunctionName,         /* __in     LPCSTR pFunctionName                */
        "ps_4_0",                           /* __in     LPCSTR pProfile                     */
        D3D10_SHADER_OPTIMIZATION_LEVEL2,   /* __in     UINT Flags1                         */
        0,                                  /* __in     UINT Flags2                         */
        NULL,                               /* __in     ID3DX11ThreadPump *pPump            */
        &this->fragmentShaderCode,          /* __out    ID3D10Blob **ppShader               */
        &errorBlob,                         /* __out    ID3D10Blob **ppErrorMsgs            */
        NULL                                /* __out    HRESULT *pHResult                   */
    );
    CE_ASSERT(SUCCEEDED(result), "D3D11GpuProgram::D3D11GpuProgram(): failed to compile fragment shader '%s' in file '%s'\n", fragmentShaderFunctionName, fileName);

    result = D3D11RenderSystem::Instance()->GetDevice()->CreatePixelShader(
        this->fragmentShaderCode->GetBufferPointer(),
        this->fragmentShaderCode->GetBufferSize(),
        NULL,
        &this->fragmentShader
    );
    CE_ASSERT(SUCCEEDED(result), "D3D11GpuProgram::D3D11GpuProgram(): failed to create fragment shader '%s' in file '%s'\n", fragmentShaderFunctionName, fileName);

    if (errorBlob != NULL)
    {
        errorBlob->Release();
        errorBlob = NULL;
    }

    /* create constant buffer */
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.ByteWidth = 16 * sizeof(float);
    result = D3D11RenderSystem::Instance()->GetDevice()->CreateBuffer(&desc, NULL, &this->autoConstantBuffer);
    CE_ASSERT(SUCCEEDED(result), "D3D11GpuProgram::D3D11GpuProgram(): failed to create constant buffer\n");

    this->constantDefs = CE_NEW graphics::GpuNamedConstants;
    this->defaultParams = NULL;
}

//------------------------------------------------------------------------------
/**
*/
D3D11GpuProgram::~D3D11GpuProgram()
{
    CE_DELETE this->constantDefs;
    if (this->defaultParams != NULL)
    {
        CE_DELETE this->defaultParams;
    }

    if (this->autoConstantBuffer != NULL)
    {
        this->autoConstantBuffer->Release();
    }
    if (this->vertexShaderCode != NULL)
    {
        this->vertexShaderCode->Release();
    }
    if (this->vertexShader != NULL)
    {
        this->vertexShader->Release();
    }
    if (this->fragmentShaderCode != NULL)
    {
        this->fragmentShaderCode->Release();
    }
    if (this->fragmentShader != NULL)
    {
        this->fragmentShader->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
graphics::GpuProgramParameters*
D3D11GpuProgram::GetDefaultParameters()
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
D3D11GpuProgram::GetConstantDefinitions() const
{
    return this->constantDefs;
}

//------------------------------------------------------------------------------
/**
*/
graphics::GpuProgramParameters*
D3D11GpuProgram::CreateParameters()
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
ID3D10Blob*
D3D11GpuProgram::GetVertexShaderCode() const
{
    return this->vertexShaderCode;
}

//------------------------------------------------------------------------------
/**
*/
ID3D11VertexShader*
D3D11GpuProgram::GetVertexShader() const
{
    return this->vertexShader;
}

//------------------------------------------------------------------------------
/**
*/
ID3D11PixelShader*
D3D11GpuProgram::GetFragmentShader() const
{
    return this->fragmentShader;
}

//------------------------------------------------------------------------------
/**
*/
ID3D11Buffer*
D3D11GpuProgram::GetAutoConstantBuffer() const
{
    return this->autoConstantBuffer;
}

} // namespace chrissly
