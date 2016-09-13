#ifndef D3D11DEFAULTSHADERS_H_
#define D3D11DEFAULTSHADERS_H_
//------------------------------------------------------------------------------
/**
    Default shaders variations for the Direct3D 11 rendersystem

    (C) 2016 Christian Bleicher
*/

//------------------------------------------------------------------------------
/**
*/
const char* DefaultGpuProgram =
    "Texture2D texture0 : register(t0);\n"
    "SamplerState samplerLinear\n"
    "{\n"
    "    Filter = MIN_MAG_MIP_LINEAR;\n"
    "    AddressU = Wrap;\n"
    "    AddressV = Wrap;\n"
    "};\n"
    "cbuffer AutoConstantBuffer : register(b0)\n"
    "{\n"
    "    matrix worldViewProjection : packoffset(c0);\n"
    "};\n"
    "struct VertexIn\n"
    "{\n"
    "    float2 uv : TEXCOORD;\n"
    "    float3 normal : NORMAL;\n"
    "    float3 position : POSITION;\n"
    "    float4 colour : COLOR;\n"
    "};\n"
    "struct VertexOut\n"
    "{\n"
    "    float2 uv : TEXCOORD;\n"
    "    float4 position : SV_Position;\n"
    "    float4 colour : COLOR;\n"
    "};\n"
    "void DefaultVertexShader(VertexIn input, out VertexOut output)\n"
    "{\n"
    "    output.uv = input.uv;\n"
    "    output.position = mul(float4(input.position, 1.0f), worldViewProjection);\n"
    "    output.colour = input.colour;\n"
    "}\n"
    "void DefaultFragmentShader(VertexOut input, out float4 output : SV_Target)\n"
    "{\n"
    "    output = texture0.Sample(samplerLinear, input.uv);\n"
    "};\n";
//------------------------------------------------------------------------------
#endif