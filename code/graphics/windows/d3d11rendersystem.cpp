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

//------------------------------------------------------------------------------
/**
*/
D3D11RenderSystem::D3D11RenderSystem() :
    ambientLight(0x00000000),
    worldMatrix(core::Matrix4::ZERO),
    viewMatrix(core::Matrix4::ZERO),
    projectionMatrix(core::Matrix4::ZERO),
    defaultGpuProgram(NULL),
    currentGpuProgram(NULL),
    device(NULL),
    context(NULL),
    rasterState(NULL),
    depthStencilState(NULL),
    inputLayout(NULL)
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

    /* create a rasterizer state object that tells the rasterizer stage how to behave */
    D3D11_RASTERIZER_DESC rasterDesc = {D3D11_FILL_SOLID, D3D11_CULL_NONE, TRUE, 0, 0.0f, 0.0f, TRUE, FALSE, FALSE, FALSE};
    result = this->device->CreateRasterizerState(&rasterDesc, &this->rasterState);
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderSystem::_Initialise(): failed to create rasterizer state\n");
    this->context->RSSetState(this->rasterState);

    /* create depth stencil state */
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    result = this->device->CreateDepthStencilState(&depthStencilDesc, &this->depthStencilState);
    CE_ASSERT(SUCCEEDED(result), "D3D11RenderSystem::_Initialise(): failed to create depth stencil state\n");
    this->context->OMSetDepthStencilState(this->depthStencilState, 1);

    /* create default gpu program */
    this->defaultGpuProgram = CE_NEW D3D11GpuProgram(DefaultGpuProgram, "defaultshader.fx", "DefaultVertexShader", "DefaultFragmentShader");
    this->currentGpuProgram = this->defaultGpuProgram;

    /* create input layout */
    D3D11_INPUT_ELEMENT_DESC inputDesc[] =
    {
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    result = this->device->CreateInputLayout(
        inputDesc, _countof(inputDesc),
        this->currentGpuProgram->GetVertexShaderCode()->GetBufferPointer(),
        this->currentGpuProgram->GetVertexShaderCode()->GetBufferSize(),
        &this->inputLayout
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
    this->currentGpuProgram = NULL;

    if (this->inputLayout != NULL)
    {
        this->inputLayout->Release();
        this->inputLayout = NULL;
    }
    if (this->depthStencilState != NULL)
    {
        this->depthStencilState->Release();
        this->depthStencilState = NULL;
    }
    if (this->rasterState != NULL)
    {
        this->rasterState->Release();
        this->rasterState = NULL;
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
    target->GetPlatformSpecificAttribute("RenderTargetView", &renderTargetView);
    ID3D11DepthStencilView* depthStencilView = NULL;
    target->GetPlatformSpecificAttribute("DepthStencilView", &depthStencilView);

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

    FLOAT clearColour[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    D3D11Mappings::Get(vp->GetBackgroundColour(), clearColour[0], clearColour[1], clearColour[2], clearColour[3]);
    graphics::RenderTarget* target = vp->GetTarget();
    ID3D11RenderTargetView* renderTargetView = NULL;
    target->GetPlatformSpecificAttribute("RenderTargetView", &renderTargetView);
    ID3D11DepthStencilView* depthStencilView = NULL;
    target->GetPlatformSpecificAttribute("DepthStencilView", &depthStencilView);

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
D3D11RenderSystem::_SetTextureMatrix(const core::Matrix4& xform)
{
    UNREFERENCED_PARAMETER(xform);
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderSystem::_Render(graphics::SubEntity* renderable)
{
    /* update auto constants */
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(mappedResource));
    ID3D11Buffer* autoConstantBuffer = this->currentGpuProgram->GetAutoConstantBuffer();
    this->context->Map(autoConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    core::Matrix4 m = this->projectionMatrix * this->viewMatrix * this->worldMatrix;
    memcpy(mappedResource.pData, m[0], 16 * sizeof(float));
    this->context->Unmap(autoConstantBuffer, 0);
    this->context->VSSetConstantBuffers(0, 1, &autoConstantBuffer);

    if (graphics::VAT_MORPH == renderable->GetSubMesh()->GetVertexAnimationType())
    {

    }
    else
    {
        graphics::HardwareVertexBuffer* vertexBuffer = renderable->GetSubMesh()->vertexData->vertexBuffer;
        ID3D11Buffer* d3d11Buffer = vertexBuffer->GetD3D11Buffer();
        UINT stride = vertexBuffer->GetBytesPerVertex(), offset = 0;
        this->context->IASetVertexBuffers(0, 1, &d3d11Buffer, &stride, &offset);
        this->context->Draw(vertexBuffer->GetNumVertices(), 0);
    }
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

    }
    else
    {

    }

    /* culling mode */
    switch (pass->GetCullingMode())
    {
        case graphics::CULL_NONE:
            break;
        case graphics::CULL_CLOCKWISE:
            break;
        case graphics::CULL_ANTICLOCKWISE:
            break;
    }

    /* set gpu program to use */
    if (pass->IsProgrammable())
    {

    }
    else
    {
        this->context->IASetInputLayout(this->inputLayout);
        this->context->VSSetShader(this->defaultGpuProgram->GetVertexShader(), NULL, 0);
        this->context->PSSetShader(this->defaultGpuProgram->GetFragmentShader(), NULL, 0);
    }

    /* texture unit parameters */
    unsigned short textureUnitState;
    for (textureUnitState = 0; textureUnitState < pass->GetNumTextureUnitStates() && textureUnitState < 1; ++textureUnitState)
    {
        ID3D11ShaderResourceView* shaderResourceView = pass->GetTextureUnitState(textureUnitState)->GetTexture()->GetShaderResourceView();
        this->context->PSSetShaderResources(0, 1, &shaderResourceView);
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
            UNREFERENCED_PARAMETER(light);
            ++lightIndex;
            it = it->next;
        }
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
void
D3D11RenderSystem::_NotifyMorphKeyFrameBuild()
{

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

} // namespace chrissly
