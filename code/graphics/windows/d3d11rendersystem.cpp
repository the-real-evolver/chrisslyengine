//------------------------------------------------------------------------------
//  d3d11rendersystem.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "d3d11rendersystem.h"
#include "d3d11mappings.h"
#include "d3d11defaultshaders.h"
#include "light.h"
#include "debug.h"

namespace chrissly
{

D3D11RenderSystem* D3D11RenderSystem::Singleton = NULL;

static const unsigned int MaxLights = 4;

//------------------------------------------------------------------------------
/**
*/
D3D11RenderSystem::D3D11RenderSystem() :
    ambientLight(0x00000000),
    worldMatrix(core::Matrix4::ZERO),
    viewMatrix(core::Matrix4::ZERO),
    projectionMatrix(core::Matrix4::ZERO),
    defaultGpuProgram(NULL),
    defaultGpuProgramFog(NULL),
    defaultGpuProgramLit(NULL),
    defaultGpuProgramLitFog(NULL),
    defaultGpuProgramMorphAnim(NULL),
    currentGpuProgram(NULL),
    device(NULL),
    context(NULL),
    inputLayout(NULL),
    inputLayoutMorphAnim(NULL),
    stateCache(NULL)
{
    Singleton = this;
    this->viewPort = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
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
graphics::RenderWindow*
D3D11RenderSystem::_Initialise(void* customParams)
{
    /* create device */
    D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_10_1};

    HRESULT result = D3D11CreateDevice(
        NULL,                       /* __in     IDXGIAdapter *pAdapter  (use NULL for the first adapter)    */
        D3D_DRIVER_TYPE_HARDWARE,   /* __in     D3D_DRIVER_TYPE DriverType                                  */
        NULL,                       /* __in     HMODULE Software                                            */
        0,                          /* __in     UINT Flags                                                  */
        featureLevels,              /* __in     const D3D_FEATURE_LEVEL *pFeatureLevels                     */
        _countof(featureLevels),    /* __in     UINT FeatureLevels                                          */
        D3D11_SDK_VERSION,          /* __in     UINT SDKVersion                                             */
        &this->device,              /* __out    ID3D11Device **ppDevice                                     */
        NULL,                       /* __out    D3D_FEATURE_LEVEL *pFeatureLevel                            */
        &this->context              /* __out    ID3D11DeviceContext **ppImmediateContext                    */
    );
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderSystem::_Initialise(): failed to create d3d11 device\n");

    /* initialise rasterizer state object that tells the rasterizer stage how to behave */
    ZeroMemory(&this->currentRasterState, sizeof(this->currentRasterState));
    this->currentRasterState.FillMode = D3D11_FILL_SOLID;
    this->currentRasterState.CullMode = D3D11_CULL_NONE;
    this->currentRasterState.FrontCounterClockwise = TRUE;
    this->currentRasterState.DepthBias = 0;
    this->currentRasterState.DepthBiasClamp = 0.0f;
    this->currentRasterState.SlopeScaledDepthBias = 0.0f;
    this->currentRasterState.DepthClipEnable = TRUE;
    this->currentRasterState.ScissorEnable = FALSE;
    this->currentRasterState.MultisampleEnable = FALSE;
    this->currentRasterState.AntialiasedLineEnable = FALSE;

    /* initialise depth stencil state */
    ZeroMemory(&this->currentDepthStencilState, sizeof(this->currentDepthStencilState));
    this->currentDepthStencilState.DepthEnable = true;
    this->currentDepthStencilState.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    this->currentDepthStencilState.DepthFunc = D3D11_COMPARISON_LESS;
    this->currentDepthStencilState.StencilEnable = true;
    this->currentDepthStencilState.StencilReadMask = 0xFF;
    this->currentDepthStencilState.StencilWriteMask = 0xFF;
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
    this->currentBlendState.RenderTarget[0].BlendEnable = TRUE;
    this->currentBlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    this->currentBlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    this->currentBlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    this->currentBlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    this->currentBlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    this->currentBlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    this->currentBlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    /* initialise sampler state */
    ZeroMemory(&this->currentSamplerState, sizeof(this->currentSamplerState));
    this->currentSamplerState.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    this->currentSamplerState.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    this->currentSamplerState.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    this->currentSamplerState.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    this->currentSamplerState.MipLODBias = 0.0f;
    this->currentSamplerState.MaxAnisotropy = 1;
    this->currentSamplerState.ComparisonFunc = D3D11_COMPARISON_NEVER;
    this->currentSamplerState.BorderColor[0] = 1.0f;
    this->currentSamplerState.BorderColor[1] = 1.0f;
    this->currentSamplerState.BorderColor[2] = 1.0f;
    this->currentSamplerState.BorderColor[3] = 1.0f;
    this->currentSamplerState.MinLOD = -FLT_MAX;
    this->currentSamplerState.MaxLOD = FLT_MAX;

    /* create renderstate cache */
    this->stateCache = CE_NEW D3D11StateCache();

    /* create default gpu programs */
    this->defaultGpuProgram = CE_NEW D3D11GpuProgram(DefaultGpuProgram, "defaultshader.fx", "DefaultVertexShader", "DefaultFragmentShader");
    this->defaultGpuProgramFog = CE_NEW D3D11GpuProgram(DefaultGpuProgramFog, "defaultshaderfog.fx", "DefaultVertexShader", "DefaultFragmentShader");
    this->defaultGpuProgramLit = CE_NEW D3D11GpuProgram(DefaultGpuProgramLit, "defaultshaderlit.fx", "DefaultVertexShader", "DefaultFragmentShader");
    this->defaultGpuProgramLitFog = CE_NEW D3D11GpuProgram(DefaultGpuProgramLitFog, "defaultshaderlitfog.fx", "DefaultVertexShader", "DefaultFragmentShader");
    this->defaultGpuProgramMorphAnim = CE_NEW D3D11GpuProgram(DefaultGpuProgramMorphAnim, "defaultshadermorphanim.fx", "DefaultVertexShader", "DefaultFragmentShader");
    this->currentGpuProgram = this->defaultGpuProgram;

    /* create default input layout */
    D3D11_INPUT_ELEMENT_DESC inputDesc[] =
    {
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    result = this->device->CreateInputLayout(
        inputDesc, _countof(inputDesc),
        this->currentGpuProgram->GetVertexShaderCode()->GetBufferPointer(),
        this->currentGpuProgram->GetVertexShaderCode()->GetBufferSize(),
        &this->inputLayout
    );
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderSystem::_Initialise(): failed to create input layout\n");

    /* create default input layout for morph animation */
    D3D11_INPUT_ELEMENT_DESC inputDescMorphAnim[] =
    {
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    1, DXGI_FORMAT_R8G8B8A8_UNORM,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   1, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    result = this->device->CreateInputLayout(
        inputDescMorphAnim, _countof(inputDescMorphAnim),
        this->defaultGpuProgramMorphAnim->GetVertexShaderCode()->GetBufferPointer(),
        this->defaultGpuProgramMorphAnim->GetVertexShaderCode()->GetBufferSize(),
        &this->inputLayoutMorphAnim
    );
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderSystem::_Initialise(): failed to create input layout\n");

    /* setup default context states */
    this->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    /* create default render window */
    graphics::RenderWindow* renderWindow = CE_NEW graphics::RenderWindow((HINSTANCE)customParams, this->device);
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
    this->currentGpuProgram = NULL;

    CE_DELETE this->stateCache;
    this->stateCache = NULL;

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
D3D11RenderSystem::_SetRenderTarget(graphics::RenderTarget* target)
{
    ID3D11RenderTargetView* renderTargetView = NULL;
    ID3D11DepthStencilView* depthStencilView = NULL;
    if ('DXGW' == target->GetType())
    {
        DXGIRenderWindow* renderWindow = (DXGIRenderWindow*)target;
        renderTargetView = renderWindow->GetRenderTargetView();
        depthStencilView = renderWindow->GetDepthStencilView();
    }

    this->context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::_SetViewport(graphics::Viewport* vp)
{
    this->viewPort.TopLeftX = (FLOAT)vp->GetActualLeft();
    this->viewPort.TopLeftY = (FLOAT)vp->GetActualTop();
    this->viewPort.Width = (FLOAT)vp->GetActualWidth();
    this->viewPort.Height = (FLOAT)vp->GetActualHeight();
    this->viewPort.MinDepth = 0.0f;
    this->viewPort.MaxDepth = 1.0f;

    this->context->RSSetViewports(1, &this->viewPort);

    ID3D11RenderTargetView* renderTargetView = NULL;
    ID3D11DepthStencilView* depthStencilView = NULL;
    graphics::RenderTarget* target = vp->GetTarget();
    if ('DXGW' == target->GetType())
    {
        DXGIRenderWindow* renderWindow = (DXGIRenderWindow*)target;
        renderTargetView = renderWindow->GetRenderTargetView();
        depthStencilView = renderWindow->GetDepthStencilView();
    }
    else
    {
        return;
    }
    FLOAT clearColour[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    D3D11Mappings::Get(vp->GetBackgroundColour(), clearColour[0], clearColour[1], clearColour[2], clearColour[3]);

    this->context->ClearRenderTargetView(renderTargetView, clearColour);
    this->context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::_SetWorldMatrix(const core::Matrix4& m)
{
    this->worldMatrix = m;
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::_SetViewMatrix(const core::Matrix4& m)
{
    this->viewMatrix = m;

    /* right-handed to left-handed conversion */
    this->viewMatrix[2][0] = -this->viewMatrix[2][0];
    this->viewMatrix[2][1] = -this->viewMatrix[2][1];
    this->viewMatrix[2][2] = -this->viewMatrix[2][2];
    this->viewMatrix[2][3] = -this->viewMatrix[2][3];
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::_SetProjectionMatrix(const core::Matrix4& m)
{
    this->projectionMatrix = m;

    /* convert depth range from [-1, +1] to [0, 1] */
    this->projectionMatrix[2][0] = (this->projectionMatrix[2][0] + this->projectionMatrix[3][0]) * 0.5f;
    this->projectionMatrix[2][1] = (this->projectionMatrix[2][1] + this->projectionMatrix[3][1]) * 0.5f;
    this->projectionMatrix[2][2] = (this->projectionMatrix[2][2] + this->projectionMatrix[3][2]) * 0.5f;
    this->projectionMatrix[2][3] = (this->projectionMatrix[2][3] + this->projectionMatrix[3][3]) * 0.5f;

    /* right-handed to left-handed conversion */
    this->projectionMatrix[0][2] = -this->projectionMatrix[0][2];
    this->projectionMatrix[1][2] = -this->projectionMatrix[1][2];
    this->projectionMatrix[2][2] = -this->projectionMatrix[2][2];
    this->projectionMatrix[3][2] = -this->projectionMatrix[3][2];
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::_SetTextureMatrix(const core::Matrix4& m)
{
    CE_UNREFERENCED_PARAMETER(m);
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::_Render(graphics::SubEntity* renderable)
{
    /* update auto constants */
    graphics::GpuProgramParameters* params = this->currentGpuProgram->GetDefaultParameters();
    core::Matrix4 m = this->projectionMatrix * this->viewMatrix * this->worldMatrix;
    params->SetAutoConstant(graphics::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX, m);
    params->SetAutoConstant(graphics::GpuProgramParameters::ACT_WORLD_MATRIX, this->worldMatrix);

    graphics::HardwareVertexBuffer* vertexBuffer = NULL;
    if (graphics::VAT_MORPH == renderable->GetSubMesh()->GetVertexAnimationType())
    {
        this->context->IASetInputLayout(this->inputLayoutMorphAnim);
        vertexBuffer = renderable->_GetHardwareVertexAnimVertexData()->vertexBuffer;
        params->SetAutoConstant(graphics::GpuProgramParameters::ACT_MORPH_WEIGHT, renderable->GetMorphWeight());
    }
    else
    {
        this->context->IASetInputLayout(this->inputLayout);
        vertexBuffer = renderable->GetSubMesh()->vertexData->vertexBuffer;
    }
    this->currentGpuProgram->UpdatePerObjectConstantBuffers();
    ID3D11Buffer* d3d11Buffer = vertexBuffer->GetD3D11Buffer();
    UINT stride = vertexBuffer->GetBytesPerVertex(), offset = 0;
    this->context->IASetVertexBuffers(0, 1, &d3d11Buffer, &stride, &offset);
    this->context->Draw(vertexBuffer->GetNumVertices(), 0);
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::_BeginFrame()
{

}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::_EndFrame()
{

}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::_SetPass(graphics::Pass* pass)
{
    /* scene blending parameters */
    if (pass->GetSceneBlendingEnabled())
    {
        this->currentBlendState.RenderTarget[0].BlendEnable = TRUE;
        this->currentBlendState.RenderTarget[0].SrcBlend = D3D11Mappings::Get(pass->GetSourceBlendFactor());
        this->currentBlendState.RenderTarget[0].DestBlend = D3D11Mappings::Get(pass->GetDestBlendFactor());
        this->currentBlendState.RenderTarget[0].BlendOp = D3D11Mappings::Get(pass->GetSceneBlendingOperation());
    }
    else
    {
        this->currentBlendState.RenderTarget[0].BlendEnable = FALSE;
    }
    ID3D11BlendState* blendState = this->stateCache->GetBlendState(this->currentBlendState);
    float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    this->context->OMSetBlendState(blendState, blendFactor, 0xffffffff);

    /* depth check and depth write */
    this->currentDepthStencilState.DepthEnable = pass->GetDepthCheckEnabled();
    this->currentDepthStencilState.DepthWriteMask = pass->GetDepthWriteEnabled() ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    ID3D11DepthStencilState* depthStencilState = this->stateCache->GetDepthStencilState(this->currentDepthStencilState);
    this->context->OMSetDepthStencilState(depthStencilState, 1);

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
    ID3D11RasterizerState* rasterState = this->stateCache->GetRasterizerState(this->currentRasterState);
    this->context->RSSetState(rasterState);

    /* set gpu program to use */
    if (pass->IsProgrammable())
    {
        this->currentGpuProgram = pass->GetGpuProgram();
    }
    else
    {
        bool lit = pass->GetLightingEnabled();
        bool fog = (graphics::FOG_LINEAR == pass->GetFogMode());
        if (!fog && !lit)
        {
            this->currentGpuProgram = this->defaultGpuProgram;
        }
        else if (fog && !lit)
        {
            this->currentGpuProgram = this->defaultGpuProgramFog;
        }
        else if (!fog && lit)
        {
            this->currentGpuProgram = this->defaultGpuProgramLit;
        }
        else if (fog && lit)
        {
            this->currentGpuProgram = this->defaultGpuProgramLitFog;
        }

        graphics::GpuProgramParameters* params = this->currentGpuProgram->GetDefaultParameters();

        if (pass->GetNumTextureUnitStates() > 0)
        {
            graphics::TextureUnitState* tus = pass->GetTextureUnitState(0);
            params->SetNamedConstant("uMod", tus->GetTextureUScroll());
            params->SetNamedConstant("vMod", tus->GetTextureVScroll());
            params->SetNamedConstant("uScale", tus->GetTextureUScale());
            params->SetNamedConstant("vScale", tus->GetTextureVScale());
        }

        if (fog)
        {
            core::Vector3 fogColour;
            float alpha;
            D3D11Mappings::Get(pass->GetFogColour(), fogColour.x, fogColour.y, fogColour.z, alpha);
            params->SetNamedConstant("fogColour", fogColour);
            params->SetNamedConstant("fogMode", (int)pass->GetFogMode());
            params->SetNamedConstant("fogStart", pass->GetFogStart());
            params->SetNamedConstant("fogEnd", pass->GetFogEnd());
        }

        if (lit)
        {
            core::Matrix4 invViewMat = this->viewMatrix.Inverse();
            core::Vector3 cameraPosition(invViewMat[0][3], invViewMat[1][3], invViewMat[2][3]);
            params->SetNamedConstant("cameraPosition", cameraPosition);
            params->SetNamedConstant("lightParams", this->defaultLightShaderParams, MaxLights);
        }

        params->SetAutoConstant(graphics::GpuProgramParameters::ACT_VIEW_MATRIX, this->viewMatrix);
    }
    this->currentGpuProgram->Bind();
    this->currentGpuProgram->BindConstantBuffers();
    this->currentGpuProgram->UpdatePerPassConstantBuffers();

    /* texture unit parameters */
    unsigned short textureUnitState;
    for (textureUnitState = 0; textureUnitState < pass->GetNumTextureUnitStates(); ++textureUnitState)
    {
        graphics::TextureUnitState* tus = pass->GetTextureUnitState(textureUnitState);
        ID3D11ShaderResourceView* shaderResourceView = tus->GetTexture()->GetShaderResourceView();
        this->context->PSSetShaderResources(textureUnitState, 1, &shaderResourceView);

        this->currentSamplerState.Filter = D3D11Mappings::Get(tus->GetTextureFiltering(graphics::FT_MIN), tus->GetTextureFiltering(graphics::FT_MAG), tus->GetTextureFiltering(graphics::FT_MIP));
        this->currentSamplerState.AddressU = D3D11Mappings::Get(tus->GetTextureAddressingMode().u);
        this->currentSamplerState.AddressV = D3D11Mappings::Get(tus->GetTextureAddressingMode().v);
        ID3D11SamplerState* samplerState = this->stateCache->GetSamplerState(this->currentSamplerState);
        this->context->PSSetSamplers(0, 1, &samplerState);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::_UseLights(ce_hash_table* lights)
{
    unsigned int lightIndex = 0;
    unsigned int i;
    for (i = 0; i < lights->bucket_count; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(lights, i);
        while (it != NULL)
        {
            graphics::Light* light = (graphics::Light*)((ce_key_value_pair*)it->data)->value;

            const core::Vector3& position = light->GetPosition();
            this->defaultLightShaderParams[lightIndex][0][0] = position.x;
            this->defaultLightShaderParams[lightIndex][0][1] = position.y;
            this->defaultLightShaderParams[lightIndex][0][2] = position.z;

            float red, green, blue, alpha;
            D3D11Mappings::Get(light->GetDiffuseColour(), red, green, blue, alpha);
            this->defaultLightShaderParams[lightIndex][1][0] = red;
            this->defaultLightShaderParams[lightIndex][1][1] = green;
            this->defaultLightShaderParams[lightIndex][1][2] = blue;

            D3D11Mappings::Get(light->GetSpecularColour(), red, green, blue, alpha);
            this->defaultLightShaderParams[lightIndex][2][0] = red;
            this->defaultLightShaderParams[lightIndex][2][1] = green;
            this->defaultLightShaderParams[lightIndex][2][2] = blue;

            ++lightIndex;
            it = it->next;
        }
    }

    for (i = lightIndex; i < MaxLights; ++i)
    {
        this->defaultLightShaderParams[lightIndex] = core::Matrix4::ZERO;
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
ID3D11Device*
D3D11RenderSystem::GetDevice() const
{
    CE_ASSERT(this->device != NULL, "D3D11RenderSystem::GetDevice(): d3d11 device not valid\n");
    return this->device;
}

//------------------------------------------------------------------------------
/**
*/
ID3D11DeviceContext*
D3D11RenderSystem::GetContext() const
{
    CE_ASSERT(this->context != NULL, "D3D11RenderSystem::GetContext(): d3d11 device context not valid\n");
    return this->context;
}

} // namespace chrissly
