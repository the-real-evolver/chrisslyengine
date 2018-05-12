//------------------------------------------------------------------------------
//  dxgirenderwindow.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "dxgirenderwindow.h"
#include "debug.h"

namespace chrissly
{

static const char* const ClassName = "CHRISSLYENGINE_WINDOWS";
static const char* const WindowTitle = "ChrisslyEngine";
static const int WindowWidth = 960;
static const int WindowHeight = 544;

//------------------------------------------------------------------------------
/**
*/
DXGIRenderWindow::DXGIRenderWindow()
{

}

//------------------------------------------------------------------------------
/**
*/
DXGIRenderWindow::DXGIRenderWindow(HINSTANCE inst, ID3D11Device* const dev) :
    instance(inst),
    hwnd(NULL),
    device(dev),
    swapChain(NULL),
    renderTargetView(NULL),
    depthStencilBuffer(NULL),
    depthStencilView(NULL)
{

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
#if __DEBUG__
    ATOM id =
#endif
    RegisterClassEx(&wndClass);
    CE_ASSERT(id != 0U, "DXGIRenderWindow::Create(): failed to register class\n");

    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
    DWORD exStyle = 0U;
    RECT rect = {0, 0, WindowWidth, WindowHeight};
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
    sd.BufferDesc.Width = WindowWidth;
    sd.BufferDesc.Height = WindowHeight;
    sd.BufferDesc.RefreshRate.Numerator = 60U;
    sd.BufferDesc.RefreshRate.Denominator = 1U;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.SampleDesc.Count = 1U;
    sd.SampleDesc.Quality = 0U;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1U;
    sd.OutputWindow = this->hwnd;
    sd.Windowed = TRUE;

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
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
    depthBufferDesc.Width = WindowWidth;
    depthBufferDesc.Height = WindowHeight;
    depthBufferDesc.MipLevels = 1U;
    depthBufferDesc.ArraySize = 1U;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1U;
    depthBufferDesc.SampleDesc.Quality = 0U;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0U;
    depthBufferDesc.MiscFlags = 0U;
    result = this->device->CreateTexture2D(&depthBufferDesc, NULL, &this->depthStencilBuffer);
    CE_ASSERT(SUCCEEDED(result), "DXGIRenderWindow::Create(): failed to create depth stencil buffer\n");

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0U;
    result = this->device->CreateDepthStencilView(this->depthStencilBuffer, &depthStencilViewDesc, &this->depthStencilView);
    CE_ASSERT(SUCCEEDED(result), "DXGIRenderWindow::Create(): failed to create depth stencil view\n");

    this->buffer = NULL;
    this->width = WindowWidth;
    this->height = WindowHeight;
    this->format = graphics::PF_R8G8B8A8;
}

//------------------------------------------------------------------------------
/**
*/
void
DXGIRenderWindow::SwapBuffers()
{
#if __DEBUG__
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