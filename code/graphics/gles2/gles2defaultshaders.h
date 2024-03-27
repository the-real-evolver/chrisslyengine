#ifndef GLES2DEFAULTSHADERS_H_
#define GLES2DEFAULTSHADERS_H_
//------------------------------------------------------------------------------
/**
    Default shaders variations for the gles2 rendersystem

    (C) 2015 Christian Bleicher
*/

//------------------------------------------------------------------------------
/**
*/
const char* const DefaultVertexShader =
    "#version 100\n"
    "attribute vec2 texCoordIn;\n"
    "attribute vec4 position;\n"
    "uniform mat4 worldViewProjMatrix;\n"
    "varying vec2 texCoordOut;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = worldViewProjMatrix * position;\n"
    "    texCoordOut = texCoordIn;\n"
    "}\n";

const char* const DefaultFragmentShader =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec2 texCoordOut;\n"
    "uniform sampler2D texture;\n"
    "uniform float uMod;\n"
    "uniform float vMod;\n"
    "uniform float uScale;\n"
    "uniform float vScale;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = texture2D(texture, vec2(uScale * texCoordOut.x + uMod, vScale * texCoordOut.y + vMod));\n"
    "}\n";

//------------------------------------------------------------------------------
/**
    Fog
*/
const char* const DefaultVertexShaderFog =
    "#version 100\n"
    "attribute vec2 texCoordIn;\n"
    "attribute vec4 position;\n"
    "uniform mat4 worldMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "uniform mat4 worldViewProjMatrix;\n"
    "uniform int fogMode;\n"
    "uniform float fogStart;\n"
    "uniform float fogEnd;\n"
    "varying float fogFactor;\n"
    "varying vec2 texCoordOut;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = worldViewProjMatrix * position;\n"
    "    texCoordOut = texCoordIn;\n"
    "    if (1 == fogMode)\n"
    "    {\n"
    "        // range based linear fog\n"
    "        fogFactor = clamp((fogEnd - length(viewMatrix * worldMatrix * position)) / (fogEnd - fogStart), 0.0, 1.0);\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "        fogFactor = 1.0;\n"
    "    }\n"
    "}\n";

const char* const DefaultFragmentShaderFog =
    "#version 100\n"
    "precision mediump float;\n"
    "varying float fogFactor;\n"
    "varying vec2 texCoordOut;\n"
    "uniform sampler2D texture;\n"
    "uniform float uMod;\n"
    "uniform float vMod;\n"
    "uniform float uScale;\n"
    "uniform float vScale;\n"
    "uniform vec3 fogColour;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = mix(vec4(fogColour, 1.0), texture2D(texture, vec2(uScale * texCoordOut.x + uMod, vScale * texCoordOut.y + vMod)), fogFactor);\n"
    "}\n";

//------------------------------------------------------------------------------
/**
    Lit
*/
const char* const DefaultVertexShaderLit =
    "#version 100\n"
    "attribute vec2 texCoordIn;\n"
    "attribute vec3 normal;\n"
    "attribute vec4 position;\n"
    "uniform mat4 worldMatrix;\n"
    "uniform mat4 worldViewProjMatrix;\n"
    "varying vec2 texCoordOut;\n"
    "varying vec3 worldNormal;\n"
    "varying vec3 worldPosition;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = worldViewProjMatrix * position;\n"
    "    texCoordOut = texCoordIn;\n"
    "    worldNormal = normalize(mat3(worldMatrix) * normal);\n"
    "    worldPosition = (worldMatrix * vec4(position.xyz, 1.0)).xyz;\n"
    "}\n";

const char* const DefaultFragmentShaderLit =
    "#version 100\n"
    "precision mediump float;\n"
    "const int MaxLights = 4;\n"
    "varying vec2 texCoordOut;\n"
    "varying vec3 worldNormal;\n"
    "varying vec3 worldPosition;\n"
    "uniform sampler2D texture;\n"
    "uniform float uMod;\n"
    "uniform float vMod;\n"
    "uniform float uScale;\n"
    "uniform float vScale;\n"
    "uniform vec3 cameraPosition;\n"
    "uniform mat4 lightParams[MaxLights];\n"
    "void main()\n"
    "{\n"
    "    vec4 colour = texture2D(texture, vec2(uScale * texCoordOut.x + uMod, vScale * texCoordOut.y + vMod));\n"
    "    vec3 L;\n"
    "    vec3 diffuse = vec3(0.0, 0.0, 0.0);\n"
    "    for (int i = 0; i < MaxLights; ++i)\n"
    "    {\n"
    "        L = normalize(vec3(lightParams[i][0][0], lightParams[i][0][1], lightParams[i][0][2]) - worldPosition);\n"
    "        diffuse += vec3(lightParams[i][1][0], lightParams[i][1][1], lightParams[i][1][2]) * max(0.0, dot(L, normalize(worldNormal)));\n"
    "    }\n"
    "    colour.rgb = clamp(colour.rgb * diffuse, 0.0, 1.0);\n"
    "    gl_FragColor = colour;\n"
    "}\n";

//------------------------------------------------------------------------------
/**
    Lit, fog
*/
const char* const DefaultVertexShaderLitFog =
    "#version 100\n"
    "attribute vec2 texCoordIn;\n"
    "attribute vec3 normal;\n"
    "attribute vec4 position;\n"
    "uniform mat4 worldMatrix;\n"
    "uniform mat4 viewMatrix;\n"
    "uniform mat4 worldViewProjMatrix;\n"
    "uniform int fogMode;\n"
    "uniform float fogStart;\n"
    "uniform float fogEnd;\n"
    "varying float fogFactor;\n"
    "varying vec2 texCoordOut;\n"
    "varying vec3 worldNormal;\n"
    "varying vec3 worldPosition;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = worldViewProjMatrix * position;\n"
    "    texCoordOut = texCoordIn;\n"
    "    if (1 == fogMode)\n"
    "    {\n"
    "        // range based linear fog\n"
    "        fogFactor = clamp((fogEnd - length(viewMatrix * worldMatrix * position)) / (fogEnd - fogStart), 0.0, 1.0);\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "        fogFactor = 1.0;\n"
    "    }\n"
    "    worldNormal = normalize(mat3(worldMatrix) * normal);\n"
    "    worldPosition = (worldMatrix * vec4(position.xyz, 1.0)).xyz;\n"
    "}\n";

const char* const DefaultFragmentShaderLitFog =
    "#version 100\n"
    "precision mediump float;\n"
    "const int MaxLights = 4;\n"
    "varying float fogFactor;\n"
    "varying vec2 texCoordOut;\n"
    "varying vec3 worldNormal;\n"
    "varying vec3 worldPosition;\n"
    "uniform sampler2D texture;\n"
    "uniform float uMod;\n"
    "uniform float vMod;\n"
    "uniform float uScale;\n"
    "uniform float vScale;\n"
    "uniform vec3 fogColour;\n"
    "uniform vec3 cameraPosition;\n"
    "uniform mat4 lightParams[MaxLights];\n"
    "void main()\n"
    "{\n"
    "    vec4 colour = texture2D(texture, vec2(uScale * texCoordOut.x + uMod, vScale * texCoordOut.y + vMod));\n"
    "    vec3 L;\n"
    "    vec3 diffuse = vec3(0.0, 0.0, 0.0);\n"
    "    for (int i = 0; i < MaxLights; ++i)\n"
    "    {\n"
    "        L = normalize(vec3(lightParams[i][0][0], lightParams[i][0][1], lightParams[i][0][2]) - worldPosition);\n"
    "        diffuse += vec3(lightParams[i][1][0], lightParams[i][1][1], lightParams[i][1][2]) * max(0.0, dot(L, normalize(worldNormal)));\n"
    "    }\n"
    "    colour.rgb = clamp(colour.rgb * diffuse, 0.0, 1.0);\n"
    "    gl_FragColor = mix(vec4(fogColour, 1.0), colour, fogFactor);\n"
    "}\n";

//------------------------------------------------------------------------------
/**
    Morphanimation, Unlit
*/
const char* const DefaultVertexShaderMorphAnim =
    "#version 100\n"
    "attribute vec2 texCoordIn;\n"
    "attribute vec4 position;\n"
    "attribute vec4 positionMorphTarget;\n"
    "uniform mat4 worldViewProjMatrix;\n"
    "uniform float morphWeight;\n"
    "varying vec2 texCoordOut;\n"
    "void main()\n"
    "{\n"
    "    vec4 pos = (position + (positionMorphTarget - position) * morphWeight);\n"
    "    gl_Position = worldViewProjMatrix * pos;\n"
    "    texCoordOut = texCoordIn;\n"
    "}\n";

const char* const DefaultFragmentShaderMorphAnim =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec2 texCoordOut;\n"
    "uniform sampler2D texture;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = texture2D(texture, texCoordOut);\n"
    "}\n";

//------------------------------------------------------------------------------
/**
    Skeletal animation, Unlit
*/
const char* const DefaultVertexShaderSkeletalAnim =
    "#version 100\n"
    "attribute vec4 weights;\n"
    "attribute vec4 indices;\n"
    "attribute vec2 texCoordIn;\n"
    "attribute vec4 position;\n"
    "uniform mat4 worldViewProjMatrix;\n"
    "uniform mat4 boneMatrices[40];\n"
    "varying vec2 texCoordOut;\n"
    "void main()\n"
    "{\n"
    "    vec3 P = position.xyz;\n"
    "    if (weights.x > 0.0 || weights.y > 0.0 || weights.z > 0.0 || weights.w > 0.0)\n"
    "    {\n"
    "        P = vec3(0.0);\n"
    "        for (int i = 0; i < 4; ++i)\n"
    "        {\n"
    "            P += (boneMatrices[int(indices[i])] * position).xyz * weights[i];\n"
    "        }\n"
    "    }\n"
    "    gl_Position = worldViewProjMatrix * vec4(P.x, P.z, -P.y, 1.0);\n"
    "    texCoordOut = texCoordIn;\n"
    "}\n";

const char* const DefaultFragmentShaderSkeletalAnim =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec2 texCoordOut;\n"
    "uniform sampler2D texture;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = texture2D(texture, texCoordOut);\n"
    "}\n";

//------------------------------------------------------------------------------
/**
    Shadowcaster
*/
const char* const DefaultVertexShaderShadowCaster =
    "#version 100\n"
    "attribute vec4 position;\n"
    "uniform mat4 worldViewProjMatrix;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = worldViewProjMatrix * position;\n"
    "}\n";

const char* const DefaultFragmentShaderShadowCaster =
    "#version 100\n"
    "precision mediump float;\n"
    "uniform vec3 shadowColour;"
    "void main()\n"
    "{\n"
    "    gl_FragColor = vec4(shadowColour, 1.0);\n"
    "}\n";

//------------------------------------------------------------------------------
/**
    Shadowcaster, Transparent
*/
const char* const DefaultVertexShaderTransparentShadowCaster =
    "#version 100\n"
    "attribute vec2 texCoordIn;\n"
    "attribute vec4 position;\n"
    "uniform mat4 worldViewProjMatrix;\n"
    "varying vec2 texCoordOut;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = worldViewProjMatrix * position;\n"
    "    texCoordOut = texCoordIn;\n"
    "}\n";

const char* const DefaultFragmentShaderTransparentShadowCaster =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec2 texCoordOut;\n"
    "uniform vec3 shadowColour;"
    "uniform sampler2D texture;\n"
    "void main()\n"
    "{\n"
    "    vec4 albedo = texture2D(texture, texCoordOut);\n"
    "    if (albedo.a < 0.001) discard;\n"
    "    gl_FragColor = vec4(shadowColour, albedo.a);\n"
    "}\n";

//------------------------------------------------------------------------------
/**
    Shadowreceiver
*/
const char* const DefaultVertexShaderShadowReceiver =
    "#version 100\n"
    "attribute vec2 texCoordIn;\n"
    "attribute vec4 position;\n"
    "uniform mat4 worldViewProjMatrix;\n"
    "uniform mat4 textureMatrix;\n"
    "varying vec4 texCoordOut;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = worldViewProjMatrix * position;\n"
    "    texCoordOut = textureMatrix * position;\n"
    "}\n";

const char* const DefaultFragmentShaderShadowReceiver =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec4 texCoordOut;\n"
    "uniform sampler2D texture;\n"
    "void main()\n"
    "{\n"
    "    vec4 colour = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "    if (texCoordOut.w > 0.0)\n"
    "    {\n"
    "        vec2 uv = texCoordOut.xy / texCoordOut.w;\n"
    "        // workaround for lack of support of GL_TEXTURE_BORDER_COLOR\n"
    "        if (clamp(uv, vec2(0.01), vec2(0.99)) == uv)\n"
    "        {\n"
    "            colour = texture2D(texture, vec2(uv.x, 1.0 - uv.y));\n"
    "        }\n"
    "    }\n"
    "    gl_FragColor = colour;\n"
    "}\n";

//------------------------------------------------------------------------------
/**
    Shadowcaster, Skeleltal Animation
*/
const char* const DefaultVertexShaderShadowCasterSkeletalAnim =
    "#version 100\n"
    "attribute vec4 weights;\n"
    "attribute vec4 indices;\n"
    "attribute vec4 position;\n"
    "uniform mat4 worldViewProjMatrix;\n"
    "uniform mat4 boneMatrices[40];\n"
    "void main()\n"
    "{\n"
    "    vec3 P = position.xyz;\n"
    "    if (weights.x > 0.0 || weights.y > 0.0 || weights.z > 0.0 || weights.w > 0.0)\n"
    "    {\n"
    "        P = vec3(0.0);\n"
    "        for (int i = 0; i < 4; ++i)\n"
    "        {\n"
    "            P += (boneMatrices[int(indices[i])] * position).xyz * weights[i];\n"
    "        }\n"
    "    }\n"
    "    gl_Position = worldViewProjMatrix * vec4(P.x, P.z, -P.y, 1.0);\n"
    "}\n";

const char* const DefaultFragmentShaderShadowCasterSkeletalAnim =
    "#version 100\n"
    "precision mediump float;\n"
    "uniform vec3 shadowColour;"
    "void main()\n"
    "{\n"
    "    gl_FragColor = vec4(shadowColour, 1.0);\n"
    "}\n";

//------------------------------------------------------------------------------
#endif