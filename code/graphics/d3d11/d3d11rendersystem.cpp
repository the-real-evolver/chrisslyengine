//------------------------------------------------------------------------------
//  d3d11rendersystem.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "d3d11rendersystem.h"
#include "d3d11mappings.h"
#include "d3d11defaultshaders.h"
#include "chrisslyconfig.h"
#include "miscutils.h"
#include "light.h"
#include "entity.h"
#include "common.h"
#include "debug.h"
#include <float.h>

namespace chrissly
{

D3D11RenderSystem* D3D11RenderSystem::Singleton = NULL;

static const unsigned int MaxLights = 4U;

//------------------------------------------------------------------------------
/**
*/
D3D11RenderSystem::D3D11RenderSystem() :
    activeViewport(NULL),
    ambientLight(0x00000000),
    worldMatrix(core::Matrix4::ZERO),
    viewMatrix(core::Matrix4::ZERO),
    projectionMatrix(core::Matrix4::ZERO),
    textureMatrix(core::Matrix4::ZERO),
    defaultGpuProgram(NULL),
    defaultGpuProgramFog(NULL),
    defaultGpuProgramLit(NULL),
    defaultGpuProgramLitFog(NULL),
    defaultGpuProgramMorphAnim(NULL),
    defaultGpuProgramSkeletalAnim(NULL),
    defaultGpuProgramNoTexture(NULL),
    defaultGpuProgramFogNoTexture(NULL),
    defaultGpuProgramLitNoTexture(NULL),
    defaultGpuProgramLitFogNoTexture(NULL),
    defaultGpuProgramMorphAnimNoTexture(NULL),
    defaultGpuProgramSkeletalAnimNoTexture(NULL),
    defaultGpuProgramShadowCaster(NULL),
    defaultGpuProgramTransparentShadowCaster(NULL),
    defaultGpuProgramTransparentShadowCasterAlphaTest(NULL),
    defaultGpuProgramShadowReceiver(NULL),
    defaultGpuProgramShadowCasterMorphAnim(NULL),
    defaultGpuProgramTransparentShadowCasterMorphAnim(NULL),
    defaultGpuProgramShadowCasterSkeletalAnim(NULL),
    currentGpuProgram(NULL),
    device(NULL),
    context(NULL),
    inputLayout(NULL),
    inputLayoutMorphAnim(NULL),
    inputLayoutSkeletalAnim(NULL),
    msaaEnable(false)
{
    Singleton = this;
    this->viewPort = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    this->scissorRect[0U] = {0, 0, 0, 0};
    ZeroMemory(&this->currentRasterState, sizeof(this->currentRasterState));
    ZeroMemory(&this->currentDepthStencilState, sizeof(this->currentDepthStencilState));
    ZeroMemory(&this->currentBlendState, sizeof(this->currentBlendState));
    ZeroMemory(&this->currentSamplerState, sizeof(this->currentSamplerState));
}

//------------------------------------------------------------------------------
/**
*/
D3D11RenderSystem::~D3D11RenderSystem()
{
    Singleton = NULL;
}

//------------------------------------------------------------------------------
/**
*/
graphics::RenderWindow* const
D3D11RenderSystem::Initialise(void* const customParams)
{
    CE_ASSERT(customParams != NULL, "D3D11RenderSystem::Initialise(): customParams must not be NULL, pass a D3D11ConfigOptions pointer with a valid HINSTANCE to GraphicsSystem::Initialise()\n");
    D3D11ConfigOptions* config = (D3D11ConfigOptions*)customParams;

    /* create device */
    D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_10_1};

    HRESULT result = D3D11CreateDevice(
        NULL,                       /* __in     IDXGIAdapter *pAdapter  (use NULL for the first adapter)    */
        D3D_DRIVER_TYPE_HARDWARE,   /* __in     D3D_DRIVER_TYPE DriverType                                  */
        NULL,                       /* __in     HMODULE Software                                            */
        0U,                         /* __in     UINT Flags                                                  */
        featureLevels,              /* __in     const D3D_FEATURE_LEVEL *pFeatureLevels                     */
        _countof(featureLevels),    /* __in     UINT FeatureLevels                                          */
        D3D11_SDK_VERSION,          /* __in     UINT SDKVersion                                             */
        &this->device,              /* __out    ID3D11Device **ppDevice                                     */
        NULL,                       /* __out    D3D_FEATURE_LEVEL *pFeatureLevel                            */
        &this->context              /* __out    ID3D11DeviceContext **ppImmediateContext                    */
    );
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderSystem::Initialise(): failed to create d3d11 device\n");

    UINT msaaNumQualityLevels = 0U;
    result = this->device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4U, &msaaNumQualityLevels);
    if (FAILED(result) || msaaNumQualityLevels == 0U) config->msaaEnable = false;
    this->msaaEnable = config->msaaEnable;

    /* initialise rasterizer state object that tells the rasterizer stage how to behave */
    ZeroMemory(&this->currentRasterState, sizeof(this->currentRasterState));
    this->currentRasterState.FillMode = D3D11_FILL_SOLID;
    this->currentRasterState.CullMode = D3D11_CULL_NONE;
    this->currentRasterState.FrontCounterClockwise = TRUE;
    this->currentRasterState.DepthBias = 0;
    this->currentRasterState.DepthBiasClamp = 0.0f;
    this->currentRasterState.SlopeScaledDepthBias = 0.0f;
    this->currentRasterState.DepthClipEnable = TRUE;
    this->currentRasterState.ScissorEnable = TRUE;
    this->currentRasterState.MultisampleEnable = config->msaaEnable;
    this->currentRasterState.AntialiasedLineEnable = config->msaaEnable;

    /* initialise depth stencil state */
    ZeroMemory(&this->currentDepthStencilState, sizeof(this->currentDepthStencilState));
    this->currentDepthStencilState.DepthEnable = TRUE;
    this->currentDepthStencilState.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    this->currentDepthStencilState.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    this->currentDepthStencilState.StencilEnable = TRUE;
    this->currentDepthStencilState.StencilReadMask = 0xff;
    this->currentDepthStencilState.StencilWriteMask = 0xff;
    this->currentDepthStencilState.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    this->currentDepthStencilState.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    this->currentDepthStencilState.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    this->currentDepthStencilState.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    this->currentDepthStencilState.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    this->currentDepthStencilState.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    this->currentDepthStencilState.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    this->currentDepthStencilState.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    /* initialise blend state */
    ZeroMemory(&this->currentBlendState, sizeof(this->currentBlendState));
    this->currentBlendState.AlphaToCoverageEnable = FALSE;
    this->currentBlendState.IndependentBlendEnable = FALSE;
    this->currentBlendState.RenderTarget[0U].BlendEnable = TRUE;
    this->currentBlendState.RenderTarget[0U].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    this->currentBlendState.RenderTarget[0U].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    this->currentBlendState.RenderTarget[0U].BlendOp = D3D11_BLEND_OP_ADD;
    this->currentBlendState.RenderTarget[0U].SrcBlendAlpha = D3D11_BLEND_ZERO;
    this->currentBlendState.RenderTarget[0U].DestBlendAlpha = D3D11_BLEND_ZERO;
    this->currentBlendState.RenderTarget[0U].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    this->currentBlendState.RenderTarget[0U].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    /* initialise sampler state */
    ZeroMemory(&this->currentSamplerState, sizeof(this->currentSamplerState));
    this->currentSamplerState.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    this->currentSamplerState.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    this->currentSamplerState.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    this->currentSamplerState.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    this->currentSamplerState.MipLODBias = 0.0f;
    this->currentSamplerState.MaxAnisotropy = 1U;
    this->currentSamplerState.ComparisonFunc = D3D11_COMPARISON_NEVER;
    this->currentSamplerState.BorderColor[0U] = 1.0f;
    this->currentSamplerState.BorderColor[1U] = 1.0f;
    this->currentSamplerState.BorderColor[2U] = 1.0f;
    this->currentSamplerState.BorderColor[3U] = 1.0f;
    this->currentSamplerState.MinLOD = -FLT_MAX;
    this->currentSamplerState.MaxLOD = FLT_MAX;

    /* create default gpu programs */
    this->defaultGpuProgram = CE_NEW D3D11GpuProgram(DefaultGpuProgram, "defaultshader.fx", "DefaultVertexShader", "DefaultFragmentShader");
    this->defaultGpuProgramFog = CE_NEW D3D11GpuProgram(DefaultGpuProgramFog, "defaultshaderfog.fx", "DefaultVertexShader", "DefaultFragmentShader");
    this->defaultGpuProgramLit = CE_NEW D3D11GpuProgram(DefaultGpuProgramLit, "defaultshaderlit.fx", "DefaultVertexShader", "DefaultFragmentShader");
    this->defaultGpuProgramLitFog = CE_NEW D3D11GpuProgram(DefaultGpuProgramLitFog, "defaultshaderlitfog.fx", "DefaultVertexShader", "DefaultFragmentShader");
    this->defaultGpuProgramMorphAnim = CE_NEW D3D11GpuProgram(DefaultGpuProgramMorphAnim, "defaultshadermorphanim.fx", "DefaultVertexShader", "DefaultFragmentShader");
    const char* shaderMacrosSkeletalAnimation[] = {CE_STRINGIFY(CE_MAX_BONES_PER_SKELETON), CE_EXPAND_AND_STRINGIFY(CE_MAX_BONES_PER_SKELETON), NULL, NULL};
    this->defaultGpuProgramSkeletalAnim = CE_NEW D3D11GpuProgram(DefaultGpuProgramSkeletalAnim, "defaultshaderskeletalanimnotexture.fx", "DefaultVertexShader", "DefaultFragmentShader", shaderMacrosSkeletalAnimation);
    const char* shaderMacrosNoTexture[] = {"NO_TEXTURE", "1", NULL, NULL};
    this->defaultGpuProgramNoTexture = CE_NEW D3D11GpuProgram(DefaultGpuProgram, "defaultshadernotexture.fx", "DefaultVertexShader", "DefaultFragmentShader", shaderMacrosNoTexture);
    this->defaultGpuProgramFogNoTexture = CE_NEW D3D11GpuProgram(DefaultGpuProgramFog, "defaultshaderfognotexture.fx", "DefaultVertexShader", "DefaultFragmentShader", shaderMacrosNoTexture);
    this->defaultGpuProgramLitNoTexture = CE_NEW D3D11GpuProgram(DefaultGpuProgramLit, "defaultshaderlitnotexture.fx", "DefaultVertexShader", "DefaultFragmentShader", shaderMacrosNoTexture);
    this->defaultGpuProgramLitFogNoTexture = CE_NEW D3D11GpuProgram(DefaultGpuProgramLitFog, "defaultshaderlitfognotexture.fx", "DefaultVertexShader", "DefaultFragmentShader", shaderMacrosNoTexture);
    this->defaultGpuProgramMorphAnimNoTexture = CE_NEW D3D11GpuProgram(DefaultGpuProgramMorphAnim, "defaultshadermorphanimnotexture.fx", "DefaultVertexShader", "DefaultFragmentShader", shaderMacrosNoTexture);
    const char* shaderMacrosSkeletalAnimationNoTexture[] = {"NO_TEXTURE", "1", CE_STRINGIFY(CE_MAX_BONES_PER_SKELETON), CE_EXPAND_AND_STRINGIFY(CE_MAX_BONES_PER_SKELETON), NULL, NULL};
    this->defaultGpuProgramSkeletalAnimNoTexture = CE_NEW D3D11GpuProgram(DefaultGpuProgramSkeletalAnim, "defaultshaderskeletalanimnotexture.fx", "DefaultVertexShader", "DefaultFragmentShader", shaderMacrosSkeletalAnimationNoTexture);
    this->defaultGpuProgramShadowCaster = CE_NEW D3D11GpuProgram(DefaultGpuProgramShadowCaster, "defaultshadershadowcaster.fx", "DefaultVertexShader", "DefaultFragmentShader");
    this->defaultGpuProgramTransparentShadowCaster = CE_NEW D3D11GpuProgram(DefaultGpuProgramTransparentShadowCaster, "defaultshadertransparentshadowcaster.fx", "DefaultVertexShader", "DefaultFragmentShader");
    const char* shaderMacrosAlphaTest[] = {"ALPHA_TEST", "1", NULL, NULL};
    this->defaultGpuProgramTransparentShadowCasterAlphaTest = CE_NEW D3D11GpuProgram(DefaultGpuProgramTransparentShadowCaster, "defaultshadertransparentshadowcasteralphatest.fx", "DefaultVertexShader", "DefaultFragmentShader", shaderMacrosAlphaTest);
    this->defaultGpuProgramShadowReceiver = CE_NEW D3D11GpuProgram(DefaultGpuProgramShadowReceiver, "defaultshadershadowreceiver.fx", "DefaultVertexShader", "DefaultFragmentShader");
    this->defaultGpuProgramShadowCasterMorphAnim = CE_NEW D3D11GpuProgram(DefaultGpuProgramShadowCasterMorphAnim, "defaultshadershadowcastermorphanim.fx", "DefaultVertexShader", "DefaultFragmentShader");
    this->defaultGpuProgramTransparentShadowCasterMorphAnim = CE_NEW D3D11GpuProgram(DefaultGpuProgramTransparentShadowCasterMorphAnim, "defaultshadertransparentshadowcastermorphanim.fx", "DefaultVertexShader", "DefaultFragmentShader");
    this->defaultGpuProgramShadowCasterSkeletalAnim = CE_NEW D3D11GpuProgram(DefaultGpuProgramShadowCasterSkeletalAnim, "defaultshadershadowcasterskeletalanim.fx", "DefaultVertexShader", "DefaultFragmentShader", shaderMacrosSkeletalAnimation);
    this->currentGpuProgram = this->defaultGpuProgram;

    /* create default input layout */
    D3D11_INPUT_ELEMENT_DESC inputDesc[] =
    {
        {"TEXCOORD", 0U, DXGI_FORMAT_R32G32_FLOAT,    0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U},
        {"COLOR",    0U, DXGI_FORMAT_R8G8B8A8_UNORM,  0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U},
        {"NORMAL",   0U, DXGI_FORMAT_R32G32B32_FLOAT, 0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U},
        {"POSITION", 0U, DXGI_FORMAT_R32G32B32_FLOAT, 0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U}
    };
    result = this->device->CreateInputLayout(
        inputDesc, _countof(inputDesc),
        this->defaultGpuProgram->GetVertexShaderCode()->GetBufferPointer(),
        this->defaultGpuProgram->GetVertexShaderCode()->GetBufferSize(),
        &this->inputLayout
    );
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderSystem::Initialise(): failed to create input layout\n");

    /* create default input layout for morph animation */
    D3D11_INPUT_ELEMENT_DESC inputDescMorphAnim[] =
    {
        {"TEXCOORD", 0U, DXGI_FORMAT_R32G32_FLOAT,    0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U},
        {"COLOR",    0U, DXGI_FORMAT_R8G8B8A8_UNORM,  0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U},
        {"NORMAL",   0U, DXGI_FORMAT_R32G32B32_FLOAT, 0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U},
        {"POSITION", 0U, DXGI_FORMAT_R32G32B32_FLOAT, 0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U},
        {"TEXCOORD", 1U, DXGI_FORMAT_R32G32_FLOAT,    0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U},
        {"COLOR",    1U, DXGI_FORMAT_R8G8B8A8_UNORM,  0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U},
        {"NORMAL",   1U, DXGI_FORMAT_R32G32B32_FLOAT, 0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U},
        {"POSITION", 1U, DXGI_FORMAT_R32G32B32_FLOAT, 0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U}
    };
    result = this->device->CreateInputLayout(
        inputDescMorphAnim, _countof(inputDescMorphAnim),
        this->defaultGpuProgramMorphAnim->GetVertexShaderCode()->GetBufferPointer(),
        this->defaultGpuProgramMorphAnim->GetVertexShaderCode()->GetBufferSize(),
        &this->inputLayoutMorphAnim
    );
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderSystem::Initialise(): failed to create input layout\n");

    /* create default input layout for skeletal animation */
    D3D11_INPUT_ELEMENT_DESC inputDescSkeletalAnim[] =
    {
        {"BLENDWEIGHT",     0U, DXGI_FORMAT_R32G32B32A32_FLOAT,    0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U},
        {"BLENDINDICES",    0U, DXGI_FORMAT_R32G32B32A32_UINT,     0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U},
        {"TEXCOORD",        0U, DXGI_FORMAT_R32G32_FLOAT,          0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U},
        {"COLOR",           0U, DXGI_FORMAT_R8G8B8A8_UNORM,        0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U},
        {"NORMAL",          0U, DXGI_FORMAT_R32G32B32_FLOAT,       0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U},
        {"POSITION",        0U, DXGI_FORMAT_R32G32B32_FLOAT,       0U, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0U}
    };
    result = this->device->CreateInputLayout(
        inputDescSkeletalAnim, _countof(inputDescSkeletalAnim),
        this->defaultGpuProgramSkeletalAnimNoTexture->GetVertexShaderCode()->GetBufferPointer(),
        this->defaultGpuProgramSkeletalAnimNoTexture->GetVertexShaderCode()->GetBufferSize(),
        &this->inputLayoutSkeletalAnim
    );
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderSystem::Initialise(): failed to create input layout\n");

    /* setup default context states */
    this->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    /* create default render window */
    graphics::RenderWindow* renderWindow = CE_NEW graphics::RenderWindow(config, this->device);
    renderWindow->Create();

    return renderWindow;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::Shutdown()
{
    CE_DELETE this->defaultGpuProgram;
    this->defaultGpuProgram = NULL;
    CE_DELETE this->defaultGpuProgramFog;
    this->defaultGpuProgramFog = NULL;
    CE_DELETE this->defaultGpuProgramLit;
    this->defaultGpuProgramLit = NULL;
    CE_DELETE this->defaultGpuProgramLitFog;
    this->defaultGpuProgramLitFog = NULL;
    CE_DELETE this->defaultGpuProgramMorphAnim;
    this->defaultGpuProgramMorphAnim = NULL;
    CE_DELETE this->defaultGpuProgramSkeletalAnim;
    this->defaultGpuProgramSkeletalAnim = NULL;
    CE_DELETE this->defaultGpuProgramNoTexture;
    this->defaultGpuProgramNoTexture = NULL;
    CE_DELETE this->defaultGpuProgramFogNoTexture;
    this->defaultGpuProgramFogNoTexture = NULL;
    CE_DELETE this->defaultGpuProgramLitNoTexture;
    this->defaultGpuProgramLitNoTexture = NULL;
    CE_DELETE this->defaultGpuProgramLitFogNoTexture;
    this->defaultGpuProgramLitFogNoTexture = NULL;
    CE_DELETE this->defaultGpuProgramMorphAnimNoTexture;
    this->defaultGpuProgramMorphAnimNoTexture = NULL;
    CE_DELETE this->defaultGpuProgramSkeletalAnimNoTexture;
    this->defaultGpuProgramSkeletalAnimNoTexture = NULL;
    CE_DELETE this->defaultGpuProgramShadowCaster;
    this->defaultGpuProgramShadowCaster = NULL;
    CE_DELETE this->defaultGpuProgramTransparentShadowCaster;
    this->defaultGpuProgramTransparentShadowCaster = NULL;
    CE_DELETE this->defaultGpuProgramTransparentShadowCasterAlphaTest;
    this->defaultGpuProgramTransparentShadowCasterAlphaTest = NULL;
    CE_DELETE this->defaultGpuProgramShadowReceiver;
    this->defaultGpuProgramShadowReceiver = NULL;
    CE_DELETE this->defaultGpuProgramShadowCasterMorphAnim;
    this->defaultGpuProgramShadowCasterMorphAnim = NULL;
    CE_DELETE this->defaultGpuProgramTransparentShadowCasterMorphAnim;
    this->defaultGpuProgramTransparentShadowCasterMorphAnim = NULL;
    CE_DELETE this->defaultGpuProgramShadowCasterSkeletalAnim;
    this->defaultGpuProgramShadowCasterSkeletalAnim = NULL;

    this->currentGpuProgram = NULL;

    if (this->inputLayout != NULL)
    {
        this->inputLayout->Release();
        this->inputLayout = NULL;
    }
    if (this->inputLayoutMorphAnim != NULL)
    {
        this->inputLayoutMorphAnim->Release();
        this->inputLayoutMorphAnim = NULL;
    }
    if (this->inputLayoutSkeletalAnim != NULL)
    {
        this->inputLayoutSkeletalAnim->Release();
        this->inputLayoutSkeletalAnim = NULL;
    }
    if (this->context != NULL)
    {
        this->context->Release();
        this->context = NULL;
    }
    if (this->device != NULL)
    {
        this->device->Release();
        this->device = NULL;
    }

    CE_LOG("D3D11RenderSystem::Shutdown()\n");
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::SetRenderTarget(graphics::RenderTarget* const target)
{
    ID3D11RenderTargetView* renderTargetView = NULL;
    ID3D11DepthStencilView* depthStencilView = NULL;
    if ('DXGW' == target->GetType())
    {
        DXGIRenderWindow* renderWindow = (DXGIRenderWindow*)target;
        renderTargetView = renderWindow->GetRenderTargetView();
        depthStencilView = renderWindow->GetDepthStencilView();
    }
    else if ('DXRT' == target->GetType())
    {
        D3D11RenderTexture* renderTexture = (D3D11RenderTexture*)target;
        renderTargetView = renderTexture->GetRenderTargetView();
        depthStencilView = renderTexture->GetDepthStencilView();
    }
    else
    {
        return;
    }

    this->context->OMSetRenderTargets(1U, &renderTargetView, depthStencilView);
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::SetViewport(graphics::Viewport* const vp)
{
    this->activeViewport = vp;

    int width = vp->GetActualWidth();
    int height = vp->GetActualHeight();
    int left = vp->GetActualLeft();
    int top = vp->GetActualTop();

    this->viewPort.TopLeftX = (FLOAT)left;
    this->viewPort.TopLeftY = (FLOAT)top;
    this->viewPort.Width = (FLOAT)width;
    this->viewPort.Height = (FLOAT)height;
    this->viewPort.MinDepth = 0.0f;
    this->viewPort.MaxDepth = 1.0f;
    this->context->RSSetViewports(1U, &this->viewPort);

    this->scissorRect[0U].left = (LONG)left;
    this->scissorRect[0U].top = (LONG)top;
    this->scissorRect[0U].right = (LONG)(left + width);
    this->scissorRect[0U].bottom = (LONG)(top + height);
    this->context->RSSetScissorRects(1U, this->scissorRect);

    if (vp->GetClearEveryFrame())
    {
        ID3D11RenderTargetView* renderTargetView = NULL;
        ID3D11DepthStencilView* depthStencilView = NULL;
        graphics::RenderTarget* target = vp->GetTarget();
        if ('DXGW' == target->GetType())
        {
            DXGIRenderWindow* renderWindow = (DXGIRenderWindow*)target;
            renderTargetView = renderWindow->GetRenderTargetView();
            depthStencilView = renderWindow->GetDepthStencilView();
        }
        else if ('DXRT' == target->GetType())
        {
            D3D11RenderTexture* renderTexture = (D3D11RenderTexture*)target;
            renderTargetView = renderTexture->GetRenderTargetView();
            depthStencilView = renderTexture->GetDepthStencilView();
        }
        else
        {
            return;
        }

        FLOAT clearColour[4U] = {0.0f, 0.0f, 0.0f, 0.0f};
        ce_colour_convert_u32_to_float(vp->GetBackgroundColour(), clearColour[0U], clearColour[1U], clearColour[2U], clearColour[3U]);
        unsigned int clearFlags = vp->GetClearBuffers();
        if (clearFlags & graphics::FBT_COLOUR)
        {
            this->context->ClearRenderTargetView(renderTargetView, clearColour);
        }
        if (depthStencilView != NULL)
        {
            this->context->ClearDepthStencilView(depthStencilView, D3D11Mappings::Get(clearFlags), 1.0f, 0U);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
graphics::Viewport* const
D3D11RenderSystem::GetViewport() const
{
    return this->activeViewport;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::SetWorldMatrix(const core::Matrix4& m)
{
    this->worldMatrix = m;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::SetViewMatrix(const core::Matrix4& m)
{
    this->viewMatrix = m;

    /* right-handed to left-handed conversion */
    this->viewMatrix[2U][0U] = -this->viewMatrix[2U][0U];
    this->viewMatrix[2U][1U] = -this->viewMatrix[2U][1U];
    this->viewMatrix[2U][2U] = -this->viewMatrix[2U][2U];
    this->viewMatrix[2U][3U] = -this->viewMatrix[2U][3U];
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::SetProjectionMatrix(const core::Matrix4& m)
{
    this->projectionMatrix = m;

    /* convert depth range from [-1, +1] to [0, 1] */
    this->projectionMatrix[2U][0U] = (this->projectionMatrix[2U][0U] + this->projectionMatrix[3U][0U]) * 0.5f;
    this->projectionMatrix[2U][1U] = (this->projectionMatrix[2U][1U] + this->projectionMatrix[3U][1U]) * 0.5f;
    this->projectionMatrix[2U][2U] = (this->projectionMatrix[2U][2U] + this->projectionMatrix[3U][2U]) * 0.5f;
    this->projectionMatrix[2U][3U] = (this->projectionMatrix[2U][3U] + this->projectionMatrix[3U][3U]) * 0.5f;

    /* right-handed to left-handed conversion */
    this->projectionMatrix[0U][2U] = -this->projectionMatrix[0U][2U];
    this->projectionMatrix[1U][2U] = -this->projectionMatrix[1U][2U];
    this->projectionMatrix[2U][2U] = -this->projectionMatrix[2U][2U];
    this->projectionMatrix[3U][2U] = -this->projectionMatrix[3U][2U];
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::SetTextureMatrix(const core::Matrix4& m)
{
    this->textureMatrix = m;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::Render(graphics::SubEntity* const renderable)
{
    /* update auto constants */
    graphics::GpuProgramParameters* params = this->currentGpuProgram->GetDefaultParameters();
    core::Matrix4 mvp = this->projectionMatrix * this->viewMatrix * this->worldMatrix;
    params->SetAutoConstant(graphics::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX, mvp);
    params->SetAutoConstant(graphics::GpuProgramParameters::ACT_WORLD_MATRIX, this->worldMatrix);
    params->SetAutoConstant(graphics::GpuProgramParameters::ACT_TEXTURE_MATRIX, this->textureMatrix);

    graphics::HardwareVertexBuffer* vertexBuffer = NULL;
    if (renderable->GetParent()->GetMesh()->GetSkeleton() != NULL)
    {
        this->context->IASetInputLayout(this->inputLayoutSkeletalAnim);
        vertexBuffer = renderable->GetSubMesh()->vertexData->vertexBuffer;
        unsigned int numBones = ce_array_size(renderable->GetParent()->GetBoneMatrices());
        CE_ASSERT(numBones <= CE_MAX_BONES_PER_SKELETON, "D3D11RenderSystem::Render(): right now only up to %i bones are supported for skeletal animation, number of bones requested: %i", CE_MAX_BONES_PER_SKELETON, numBones);
        params->SetNamedConstant("boneMatrices", renderable->GetParent()->GetBoneMatrices(), numBones);
    }
    else if (graphics::VAT_MORPH == renderable->GetSubMesh()->GetVertexAnimationType())
    {
        this->context->IASetInputLayout(this->inputLayoutMorphAnim);
        vertexBuffer = renderable->GetMorphVertexData()->vertexBuffer;
        params->SetAutoConstant(graphics::GpuProgramParameters::ACT_MORPH_WEIGHT, renderable->GetMorphWeight());
    }
    else
    {
        this->context->IASetInputLayout(this->inputLayout);
        vertexBuffer = renderable->GetSubMesh()->vertexData->vertexBuffer;
    }
    this->currentGpuProgram->UpdatePerObjectConstantBuffers();
    ID3D11Buffer* d3d11Buffer = vertexBuffer->GetD3D11Buffer();
    UINT stride = vertexBuffer->GetBytesPerVertex(), offset = 0U;
    this->context->IASetPrimitiveTopology(D3D11Mappings::Get(renderable->GetSubMesh()->topology));
    this->context->IASetVertexBuffers(0U, 1U, &d3d11Buffer, &stride, &offset);
    this->context->Draw(vertexBuffer->GetNumVertices(), 0U);
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::BeginFrame()
{

}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::EndFrame()
{

}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::SetPass(graphics::Pass* const pass)
{
    /* scene blending parameters */
    if (pass->GetSceneBlendingEnabled())
    {
        this->currentBlendState.RenderTarget[0U].BlendEnable = TRUE;
        this->currentBlendState.RenderTarget[0U].SrcBlend = D3D11Mappings::Get(pass->GetSourceBlendFactor());
        this->currentBlendState.RenderTarget[0U].DestBlend = D3D11Mappings::Get(pass->GetDestBlendFactor());
        this->currentBlendState.RenderTarget[0U].BlendOp = D3D11Mappings::Get(pass->GetSceneBlendingOperation());
    }
    else
    {
        this->currentBlendState.RenderTarget[0U].BlendEnable = FALSE;
    }
    this->currentBlendState.AlphaToCoverageEnable = this->msaaEnable && pass->GetAlphaToCoverageEnabled();
    ID3D11BlendState* blendState = NULL;
    HRESULT result = this->device->CreateBlendState(&this->currentBlendState, &blendState);
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderSystem::SetPass(): failed to create blend state\n");
    float blendFactor[4U] = {0.0f, 0.0f, 0.0f, 0.0f};
    this->context->OMSetBlendState(blendState, blendFactor, 0xffffffff);

    /* depth check and depth write */
    this->currentDepthStencilState.DepthEnable = pass->GetDepthCheckEnabled();
    this->currentDepthStencilState.DepthWriteMask = pass->GetDepthWriteEnabled() ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    ID3D11DepthStencilState* depthStencilState = NULL;
    result = this->device->CreateDepthStencilState(&this->currentDepthStencilState, &depthStencilState);
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderSystem::SetPass(): failed to create depth stencil state\n");
    this->context->OMSetDepthStencilState(depthStencilState, 1U);

    /* culling mode */
    switch (pass->GetCullingMode())
    {
        case graphics::CULL_NONE:
            this->currentRasterState.CullMode = D3D11_CULL_NONE;
            break;
        case graphics::CULL_CLOCKWISE:
            this->currentRasterState.CullMode = D3D11_CULL_BACK;
            break;
        case graphics::CULL_ANTICLOCKWISE:
            this->currentRasterState.CullMode = D3D11_CULL_FRONT;
            break;
    }
    ID3D11RasterizerState* rasterState = NULL;
    result = this->device->CreateRasterizerState(&this->currentRasterState, &rasterState);
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderSystem::SetPass(): failed to create rasterizer state\n");
    this->context->RSSetState(rasterState);

    /* texture unit parameters */
    unsigned short textureUnitState;
    for (textureUnitState = 0U; textureUnitState < pass->GetNumTextureUnitStates(); ++textureUnitState)
    {
        graphics::TextureUnitState* tus = pass->GetTextureUnitState(textureUnitState);
        ID3D11ShaderResourceView* shaderResourceView = tus->GetTexture()->GetShaderResourceView();
        this->context->PSSetShaderResources(textureUnitState, 1U, &shaderResourceView);

        this->currentSamplerState.Filter = D3D11Mappings::Get(tus->GetTextureFiltering(graphics::FT_MIN), tus->GetTextureFiltering(graphics::FT_MAG), tus->GetTextureFiltering(graphics::FT_MIP));
        this->currentSamplerState.AddressU = D3D11Mappings::Get(tus->GetTextureAddressingMode().u);
        this->currentSamplerState.AddressV = D3D11Mappings::Get(tus->GetTextureAddressingMode().v);
        ID3D11SamplerState* samplerState = NULL;
        result = this->device->CreateSamplerState(&this->currentSamplerState, &samplerState);
        CE_ASSERT(SUCCEEDED(result), "D3D11RenderSystem::SetPass(): failed to create sampler state\n");
        this->context->PSSetSamplers(textureUnitState, 1U, &samplerState);
    }

    /* set gpu program to use */
    graphics::GpuProgramParameters* params = NULL;
    if (pass->IsProgrammable())
    {
        this->currentGpuProgram = pass->GetGpuProgram();
        params = this->currentGpuProgram->GetDefaultParameters();
    }
    else
    {
        bool lit = pass->GetLightingEnabled();
        bool fog = (graphics::FOG_LINEAR == pass->GetFogMode());
        bool textured = pass->GetNumTextureUnitStates() > 0U;
        if (pass->IsSkeletalAnimationIncluded())
        {
            this->currentGpuProgram = textured ? this->defaultGpuProgramSkeletalAnim : this->defaultGpuProgramSkeletalAnimNoTexture;
        }
        else if (pass->IsMorphAnimationIncluded())
        {
            this->currentGpuProgram = textured ? this->defaultGpuProgramMorphAnim : this->defaultGpuProgramMorphAnimNoTexture;
        }
        else if (!fog && !lit)
        {
            this->currentGpuProgram = textured ? this->defaultGpuProgram : this->defaultGpuProgramNoTexture;
        }
        else if (fog && !lit)
        {
            this->currentGpuProgram = textured ? this->defaultGpuProgramFog : this->defaultGpuProgramFogNoTexture;
        }
        else if (!fog && lit)
        {
            this->currentGpuProgram = textured ? this->defaultGpuProgramLit : this->defaultGpuProgramLitNoTexture;
        }
        else if (fog && lit)
        {
            this->currentGpuProgram = textured ? this->defaultGpuProgramLitFog : this->defaultGpuProgramLitFogNoTexture;
        }

        params = this->currentGpuProgram->GetDefaultParameters();

        if (textured)
        {
            graphics::TextureUnitState* tus = pass->GetTextureUnitState(0U);
            params->SetNamedConstant("uMod", tus->GetTextureUScroll());
            params->SetNamedConstant("vMod", tus->GetTextureVScroll());
            params->SetNamedConstant("uScale", tus->GetTextureUScale());
            params->SetNamedConstant("vScale", tus->GetTextureVScale());
        }
        else
        {
            core::Quaternion ambient;
            ce_colour_convert_u32_to_float(pass->GetDiffuse(), ambient.w, ambient.x, ambient.y, ambient.z);
            params->SetNamedConstant("ambient", ambient);
        }

        if (fog)
        {
            core::Vector3 fogColour;
            float alpha;
            ce_colour_convert_u32_to_float(pass->GetFogColour(), fogColour.x, fogColour.y, fogColour.z, alpha);
            params->SetNamedConstant("fogColour", fogColour);
            params->SetNamedConstant("fogMode", (int)pass->GetFogMode());
            params->SetNamedConstant("fogStart", pass->GetFogStart());
            params->SetNamedConstant("fogEnd", pass->GetFogEnd());
        }

        if (lit)
        {
            core::Matrix4 invViewMat = this->viewMatrix.Inverse();
            core::Vector3 cameraPosition(invViewMat[0U][3U], invViewMat[1U][3U], invViewMat[2U][3U]);
            params->SetNamedConstant("cameraPosition", cameraPosition);
            float shininess = pass->GetShininess() > 0.0f ? pass->GetShininess() : 1.0f;
            this->defaultLightShaderParams[0U][2U][3U] = shininess;
            this->defaultLightShaderParams[1U][2U][3U] = shininess;
            this->defaultLightShaderParams[2U][2U][3U] = shininess;
            this->defaultLightShaderParams[3U][2U][3U] = shininess;
            params->SetNamedConstant("lightParams", this->defaultLightShaderParams, MaxLights);
        }
    }

    /* set auto params */
    params->SetAutoConstant(graphics::GpuProgramParameters::ACT_VIEW_MATRIX, this->viewMatrix);
    params->SetAutoConstant(graphics::GpuProgramParameters::ACT_PROJECTION_MATRIX, this->projectionMatrix);

    this->currentGpuProgram->Bind();
    this->currentGpuProgram->BindConstantBuffers();
    this->currentGpuProgram->UpdatePerPassConstantBuffers();
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::ProcessLights(ce_hash_table* const lights)
{
    unsigned int lightIndex = 0U;
    unsigned int i;
    for (i = 0U; i < lights->bucket_count; ++i)
    {
        ce_linked_list* it = lights->buckets[i];
        while (it != NULL)
        {
            graphics::Light* light = (graphics::Light*)((ce_key_value_pair*)it->data)->value;

            const core::Vector3& position = light->GetPosition();
            this->defaultLightShaderParams[lightIndex][0U][0U] = position.x;
            this->defaultLightShaderParams[lightIndex][0U][1U] = position.y;
            this->defaultLightShaderParams[lightIndex][0U][2U] = position.z;

            float red, green, blue, alpha;
            ce_colour_convert_u32_to_float(light->GetDiffuseColour(), red, green, blue, alpha);
            this->defaultLightShaderParams[lightIndex][1U][0U] = red;
            this->defaultLightShaderParams[lightIndex][1U][1U] = green;
            this->defaultLightShaderParams[lightIndex][1U][2U] = blue;

            ce_colour_convert_u32_to_float(light->GetSpecularColour(), red, green, blue, alpha);
            this->defaultLightShaderParams[lightIndex][2U][0U] = red;
            this->defaultLightShaderParams[lightIndex][2U][1U] = green;
            this->defaultLightShaderParams[lightIndex][2U][2U] = blue;

            this->defaultLightShaderParams[lightIndex][3U][0U] = light->GetAttenuationConstant();
            this->defaultLightShaderParams[lightIndex][3U][1U] = light->GetAttenuationLinear();
            this->defaultLightShaderParams[lightIndex][3U][2U] = light->GetAttenuationQuadric();
            this->defaultLightShaderParams[lightIndex][3U][3U] = light->GetAttenuationRange();

            ++lightIndex;
            it = it->next;
        }
    }

    for (i = lightIndex; i < MaxLights; ++i)
    {
        this->defaultLightShaderParams[i] = core::Matrix4::ZERO;

        this->defaultLightShaderParams[i][3U][0U] = 1.0f;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::SetAmbientLight(unsigned int colour)
{
    this->ambientLight = colour;
}

//------------------------------------------------------------------------------
/**
*/
D3D11GpuProgram* const
D3D11RenderSystem::GetDefaultShadowCasterGpuProgram() const
{
    CE_ASSERT(this->defaultGpuProgramShadowCaster != NULL, "D3D11RenderSystem::GetDefaultShadowCasterGpuProgram(): gpu program not valid\n");
    return this->defaultGpuProgramShadowCaster;
}

//------------------------------------------------------------------------------
/**
*/
D3D11GpuProgram* const
D3D11RenderSystem::GetDefaultTransparentShadowCasterGpuProgram() const
{
    CE_ASSERT(this->defaultGpuProgramTransparentShadowCaster != NULL, "D3D11RenderSystem::GetDefaultTransparentShadowCasterGpuProgram(): gpu program not valid\n");
    return this->defaultGpuProgramTransparentShadowCaster;
}

//------------------------------------------------------------------------------
/**
*/
D3D11GpuProgram* const
D3D11RenderSystem::GetDefaultTransparentShadowCasterAlphaTestGpuProgram() const
{
    CE_ASSERT(this->defaultGpuProgramTransparentShadowCasterAlphaTest != NULL, "D3D11RenderSystem::GetDefaultTransparentShadowCasterAlphaTestGpuProgram(): gpu program not valid\n");
    return this->defaultGpuProgramTransparentShadowCasterAlphaTest;
}

//------------------------------------------------------------------------------
/**
*/
D3D11GpuProgram* const
D3D11RenderSystem::GetDefaultShadowCasterMorphAnimGpuProgram() const
{
    CE_ASSERT(this->defaultGpuProgramShadowCasterMorphAnim != NULL, "D3D11RenderSystem::GetDefaultShadowCasterMorphAnimGpuProgram(): gpu program not valid\n");
    return this->defaultGpuProgramShadowCasterMorphAnim;
}

//------------------------------------------------------------------------------
/**
*/
D3D11GpuProgram* const
D3D11RenderSystem::GetDefaultTransparentShadowCasterMorphAnimGpuProgram() const
{
    CE_ASSERT(this->defaultGpuProgramTransparentShadowCasterMorphAnim != NULL, "D3D11RenderSystem::GetDefaultTransparentShadowCasterMorphAnimGpuProgram(): gpu program not valid\n");
    return this->defaultGpuProgramTransparentShadowCasterMorphAnim;
}

//------------------------------------------------------------------------------
/**
*/
D3D11GpuProgram* const
D3D11RenderSystem::GetDefaultShadowCasterSkeletalAnimGpuProgram() const
{
    CE_ASSERT(this->defaultGpuProgramShadowCasterSkeletalAnim != NULL, "D3D11RenderSystem::GetDefaultShadowCasterSkeletalAnimGpuProgram(): gpu program not valid\n");
    return this->defaultGpuProgramShadowCasterSkeletalAnim;
}

//------------------------------------------------------------------------------
/**
*/
D3D11GpuProgram* const
D3D11RenderSystem::GetDefaultShadowReceiverGpuProgram() const
{
    CE_ASSERT(this->defaultGpuProgramShadowReceiver != NULL, "D3D11RenderSystem::GetDefaultShadowReceiverGpuProgram(): gpu program not valid\n");
    return this->defaultGpuProgramShadowReceiver;
}

//------------------------------------------------------------------------------
/**
*/
ID3D11Device* const
D3D11RenderSystem::GetDevice() const
{
    CE_ASSERT(this->device != NULL, "D3D11RenderSystem::GetDevice(): d3d11 device not valid\n");
    return this->device;
}

//------------------------------------------------------------------------------
/**
*/
ID3D11DeviceContext* const
D3D11RenderSystem::GetContext() const
{
    CE_ASSERT(this->context != NULL, "D3D11RenderSystem::GetContext(): d3d11 device context not valid\n");
    return this->context;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::CreateDepthBuffer(UINT width, UINT height, bool msaa, ID3D11Texture2D** bufferOut, ID3D11DepthStencilView** viewOut)
{
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
    depthBufferDesc.Width = width;
    depthBufferDesc.Height = height;
    depthBufferDesc.MipLevels = 1U;
    depthBufferDesc.ArraySize = 1U;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = msaa ? 4U : 1U;
    depthBufferDesc.SampleDesc.Quality = 0U;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0U;
    depthBufferDesc.MiscFlags = 0U;
    HRESULT result = this->device->CreateTexture2D(&depthBufferDesc, NULL, bufferOut);
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderSystem::CreateDepthBuffer(): failed to create depth stencil buffer\n");

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = msaa ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0U;
    result = this->device->CreateDepthStencilView(*bufferOut, &depthStencilViewDesc, viewOut);
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderSystem::CreateDepthBuffer(): failed to create depth stencil view\n");
}

} // namespace chrissly
