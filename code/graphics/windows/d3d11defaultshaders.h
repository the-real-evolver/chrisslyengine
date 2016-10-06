#ifndef D3D11DEFAULTSHADERS_H_
#define D3D11DEFAULTSHADERS_H_
//------------------------------------------------------------------------------
/**
    Default shader variations for the Direct3D 11 rendersystem

    (C) 2016 Christian Bleicher
*/

//------------------------------------------------------------------------------
/**
*/
const char* DefaultGpuProgram =
    "Texture2D texture0 : register(t0);\n"
    "SamplerState samplerLinear : register(s0);\n"
    "cbuffer AutoConstantBuffer : register(b0)\n"
    "{\n"
    "    matrix worldViewProjection : packoffset(c0);\n"
    "};\n"
    "struct VertexIn\n"
    "{\n"
    "    float2 uv : TEXCOORD0;\n"
    "    float3 normal : NORMAL0;\n"
    "    float3 position : POSITION0;\n"
    "    float4 colour : COLOR0;\n"
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
/**
*/
const char* DefaultGpuProgramMorphAnim =
    "Texture2D texture0 : register(t0);\n"
    "SamplerState samplerLinear : register(s0);\n"
    "cbuffer AutoConstantBuffer : register(b0)\n"
    "{\n"
    "    matrix worldViewProjection : packoffset(c0);\n"
    "};\n"
    "struct VertexIn\n"
    "{\n"
    "    float2 uv : TEXCOORD0;\n"
    "    float3 normal : NORMAL0;\n"
    "    float3 position : POSITION0;\n"
    "    float4 colour : COLOR0;\n"
    "    float2 uvTarget : TEXCOORD1;\n"
    "    float3 normalTarget : NORMAL1;\n"
    "    float3 positionTarget : POSITION1;\n"
    "    float4 colourTarget : COLOR1;\n"
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