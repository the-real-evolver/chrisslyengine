#ifndef D3D11GPUPROGRAM_H_
#define D3D11GPUPROGRAM_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::D3D11GpuProgram

    (C) 2016 Christian Bleicher
*/
#include "gpuprogramparams.h"
#pragma warning(disable : 4005)
#include <d3dx11.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class D3D11GpuProgram
{
public:
    /// construct from source
    D3D11GpuProgram(const char* source, const char* fileName, const char* vertexShaderFunctionName, const char* fragmentShaderFunctionName);
    /// destructor
    ~D3D11GpuProgram();
    /// get a reference to the default parameters which are to be used for all uses of this program
    graphics::GpuProgramParameters* GetDefaultParameters();
    /// get the full list of named constants
    graphics::GpuNamedConstants* GetConstantDefinitions() const;
    /// sets the vertex and fragment shader to the device
    void Bind();
    /// set the constant buffers used by the vertex shader pipeline stage
    void BindConstantBuffers();
    /// updates per object constant buffers
    void UpdatePerObjectConstantBuffers();
    /// updates per pass constant buffers
    void UpdatePerPassConstantBuffers();

    /// gets a pointer to the vertex shader code
    ID3D10Blob* GetVertexShaderCode() const;

private:
    /// private default constructor
    D3D11GpuProgram();
    /// populate named constants
    void ExtractConstantDefs();

    graphics::GpuProgramParameters* defaultParams;
    graphics::GpuNamedConstants* constantDefs;

    ce_dynamic_array constantBuffersPerObject;
    ce_dynamic_array constantBuffersPerPass;

    ID3D10Blob* vertexShaderCode;
    ID3D10Blob* fragmentShaderCode;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* fragmentShader;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
