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

    /// gets a pointer to the vertex shader code
    ID3D10Blob* GetVertexShaderCode() const;
    /// gets a pointer to the vertex shader
    ID3D11VertexShader* GetVertexShader() const;
    /// gets a pointer to the fragment shader
    ID3D11PixelShader* GetFragmentShader() const;
    /// gets a pointer to the auto constant buffer
    ID3D11Buffer* GetAutoConstantBuffer() const;

private:
    /// private default constructor
    D3D11GpuProgram();
    /// creates a new parameters object compatible with this program definition
    graphics::GpuProgramParameters* CreateParameters();

    graphics::GpuProgramParameters* defaultParams;
    graphics::GpuNamedConstants* constantDefs;

    ID3D10Blob* vertexShaderCode;
    ID3D10Blob* fragmentShaderCode;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* fragmentShader;
    ID3D11Buffer* autoConstantBuffer;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
