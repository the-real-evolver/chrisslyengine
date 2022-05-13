#ifndef D3D11GPUPROGRAM_H_
#define D3D11GPUPROGRAM_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::D3D11GpuProgram

    (C) 2016 Christian Bleicher
*/
#include "gpuprogramparams.h"
#include "d3d11constantbuffer.h"
#include <d3d11.h>

//------------------------------------------------------------------------------
namespace chrissly
{

class D3D11GpuProgram
{
public:
    /// construct from source
    D3D11GpuProgram(const char* const source, const char* const fileName, const char* const vertexShaderFunctionName, const char* const fragmentShaderFunctionName, const char* const* const macros = NULL);
    /// destructor
    ~D3D11GpuProgram();
    /// get a reference to the default parameters which are to be used for all uses of this program
    graphics::GpuProgramParameters* const GetDefaultParameters() const;
    /// get the full list of named constants
    graphics::GpuNamedConstants* const GetConstantDefinitions() const;
    /// sets the vertex and fragment shader to the device
    void Bind();
    /// set the constant buffers used by the vertex shader pipeline stage
    void BindConstantBuffers();
    /// updates per object constant buffers
    void UpdatePerObjectConstantBuffers();
    /// updates per pass constant buffers
    void UpdatePerPassConstantBuffers();

    /// gets a pointer to the vertex shader code
    ID3D10Blob* const GetVertexShaderCode() const;

private:
    /// private default constructor
    D3D11GpuProgram();
    /// populate named constants
    void ExtractConstantDefs(ID3D10Blob* const shaderCode);

    graphics::GpuProgramParameters* defaultParams;
    graphics::GpuNamedConstants* constantDefs;

    D3D11ConstantBuffer** constantBuffersPerObject;
    D3D11ConstantBuffer** constantBuffersPerPass;

    ID3D10Blob* vertexShaderCode;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* fragmentShader;

    ce_hash_table constantBuffers;
    UINT bufferSlot;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
