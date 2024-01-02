//------------------------------------------------------------------------------
//  dxgirenderwindow.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "dxgirenderwindow.h"
#include "d3d11rendersystem.h"
#include "debug.h"

namespace chrissly
{

static const char* const ClassName = "CHRISSLYENGINE_WINDOWS";
static const char* const WindowTitle = "ChrisslyEngine";

//------------------------------------------------------------------------------
/**
*/
DXGIRenderWindow::DXGIRenderWindow(D3D11ConfigOptions* config, ID3D11Device* const dev) :
    instance(config->instance),
    hwnd(NULL),
    device(dev),
    swapChain(NULL),
    renderTargetView(NULL),
    depthStencilBuffer(NULL),
    depthStencilView(NULL),
    fullScreen(false),
    depthBuffer(true),
    msaaEnable(false)
{
    if (config->fullScreen)
    {
        this->width = GetSystemMetrics(SM_CXSCREEN);
        this->height = GetSystemMetrics(SM_CYSCREEN);
    }
    else
    {
        this->width = config->windowWidth;
        this->height = config->windowHeight;
    }
    this->fullScreen = config->fullScreen;

    this->depthBuffer = config->depthBuffer;

    this->msaaEnable = config->msaaEnable;
}

//------------------------------------------------------------------------------
/**
*/
DXGIRenderWindow::~DXGIRenderWindow()
{
    this->RemoveAllViewports();

    if (this->depthStencilView != NULL)
    {
        this->depthStencilView->Release();
    }
    if (this->depthStencilBuffer != NULL)
    {
        this->depthStencilBuffer->Release();
    }
    if (this->renderTargetView != NULL)
    {
        this->renderTargetView->Release();
    }
    if (this->swapChain != NULL)
    {
        this->swapChain->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
DXGIRenderWindow::Create()
{
    /* create native window */
    WNDCLASSEX wndClass;
    ZeroMemory(&wndClass, sizeof(wndClass));
    wndClass.cbSize = sizeof(wndClass);
    wndClass.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
    wndClass.hbrBackground = NULL;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hInstance = this->instance;
    wndClass.lpfnWndProc = &DXGIRenderWindow::WindowCallback;
    wndClass.lpszClassName = ClassName;
#if __CE_DEBUG__
    ATOM id =
#endif
    RegisterClassEx(&wndClass);
    CE_ASSERT(id != 0U, "DXGIRenderWindow::Create(): failed to register class\n");

    DWORD style = this->fullScreen ? (WS_POPUP | WS_VISIBLE) : (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE);
    DWORD exStyle = 0U;
    RECT rect = {0, 0, (LONG)this->width, (LONG)this->height};
    AdjustWindowRectEx(&rect, style, FALSE, exStyle);

    this->hwnd = CreateWindowEx(
        exStyle,                /* _In_     DWORD dwExStyle         */
        ClassName,              /* _In_opt_ LPCTSTR lpClassName     */
        WindowTitle,            /* _In_opt_ LPCTSTR lpWindowName    */
        style,                  /* _In_     DWORD dwStyle           */
        CW_USEDEFAULT,          /* _In_     int x                   */
        CW_USEDEFAULT,          /* _In_     int y                   */
        rect.right - rect.left, /* _In_     int nWidth              */
        rect.bottom - rect.top, /* _In_     int nHeight             */
        NULL,                   /* _In_opt_ HWND hWndParent         */
        NULL,                   /* _In_opt_ HMENU hMenu             */
        this->instance,         /* _In_opt_ HINSTANCE hInstance     */
        NULL                    /* _In_opt_ LPVOID lpParam          */
    );
    CE_ASSERT(this->hwnd != NULL, "DXGIRenderWindow::Create(): failed to create window\n");

    /* create swapchain */
    IDXGIDevice* dxgiDevice = NULL;
    HRESULT result = this->device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
    CE_ASSERT(SUCCEEDED(result), "DXGIRenderWindow::Create(): failed to query IDXGIDevice interface\n");
    IDXGIAdapter* dxgiAdapter = NULL;
    result = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
    CE_ASSERT(SUCCEEDED(result), "DXGIRenderWindow::Create(): failed to get IDXGIAdapter\n");
    IDXGIFactory* dxgiFactory = NULL;
    result = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
    CE_ASSERT(SUCCEEDED(result), "DXGIRenderWindow::Create(): failed to get IDXGIFactory\n");

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferDesc.Width = (UINT)this->width;
    sd.BufferDesc.Height = (UINT)this->height;
    sd.BufferDesc.RefreshRate.Numerator = 60U;
    sd.BufferDesc.RefreshRate.Denominator = 1U;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.SampleDesc.Count = this->msaaEnable ? 4U : 1U;
    sd.SampleDesc.Quality = 0U;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = this->msaaEnable ? 1U : 2U;
    sd.OutputWindow = this->hwnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = this->msaaEnable ? DXGI_SWAP_EFFECT_DISCARD : DXGI_SWAP_EFFECT_FLIP_DISCARD;

    result = dxgiFactory->CreateSwapChain(
        this->device,       /* [in]     IUnknown *pDevice               */
        &sd,                /* [in]     DXGI_SWAP_CHAIN_DESC *pDesc     */
        &this->swapChain    /* [out]    IDXGISwapChain **ppSwapChain    */
    );
    CE_ASSERT(SUCCEEDED(result), "DXGIRenderWindow::Create(): failed to create swapchain\n");

    dxgiFactory->Release();
    dxgiAdapter->Release();
    dxgiDevice->Release();

    /* get buffer of the swapchain and create its render target view */
    ID3D11Texture2D* backBuffer = NULL;
    result = this->swapChain->GetBuffer(0U, __uuidof(*backBuffer), (void**)&backBuffer);
    CE_ASSERT(SUCCEEDED(result), "DXGIRenderWindow::Create(): failed to get buffer of the swapchain\n");
    result = this->device->CreateRenderTargetView(backBuffer, NULL, &this->renderTargetView);
    CE_ASSERT(SUCCEEDED(result), "DXGIRenderWindow::Create(): failed to create rendertarget view\n");
    backBuffer->Release();

    /* create depth stencil buffer and depth stencil view */
    if (this->depthBuffer)
    {
        D3D11RenderSystem::Instance()->CreateDepthBuffer((UINT)this->width, (UINT)this->height, this->msaaEnable, &this->depthStencilBuffer, &this->depthStencilView);
    }

    this->buffer = NULL;
    this->format = graphics::PF_R8G8B8A8;
}

//------------------------------------------------------------------------------
/**
*/
void
DXGIRenderWindow::SwapBuffers()
{
#if __CE_DEBUG__
    HRESULT result =
#endif
    this->swapChain->Present(1U, 0U);
    CE_ASSERT(SUCCEEDED(result), "DXGIRenderWindow::SwapBuffers(): failed to present swapchain\n");
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
DXGIRenderWindow::GetType() const
{
    return 'DXGW';
}

//------------------------------------------------------------------------------
/**
*/
ID3D11RenderTargetView* const
DXGIRenderWindow::GetRenderTargetView() const
{
    return this->renderTargetView;
}

//------------------------------------------------------------------------------
/**
*/
ID3D11DepthStencilView* const
DXGIRenderWindow::GetDepthStencilView() const
{
    return this->depthStencilView;
}

//------------------------------------------------------------------------------
/**
*/
HWND
DXGIRenderWindow::GetWindowHandle() const
{
    return this->hwnd;
}

//------------------------------------------------------------------------------
/**
*/
LRESULT WINAPI
DXGIRenderWindow::WindowCallback(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN:
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostMessage(wnd, WM_CLOSE, 0, 0);
                    break;
            }
            return 0;
    }

    return DefWindowProc(wnd, msg, wParam, lParam);
}

} // namespace chrissly