#ifndef DXGIRENDERWINDOW_H_
#define DXGIRENDERWINDOW_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::DXGIRenderWindow

    (C) 2016 Christian Bleicher
*/
#include "rendertarget.h"
#include <windows.h>
#include <d3d11.h>

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{
    struct ConfigOptions;
}

class DXGIRenderWindow : public graphics::RenderTarget
{
public:
    /// constructor with instance handle and device
    DXGIRenderWindow(graphics::ConfigOptions* config, ID3D11Device* const dev);
    /// destructor
    ~DXGIRenderWindow();
    /// creates & displays the new window
    void Create();
    /// swaps the frame buffers to display the next frame
    void SwapBuffers();
    /// get the type of the rendertarget
    unsigned int GetType() const;

    /// gets a pointer to the rendertarget view of the swapchain's buffer
    ID3D11RenderTargetView* GetRenderTargetView() const;
    /// gets a pointer to the depth stencil view
    ID3D11DepthStencilView* GetDepthStencilView() const;

    /// gets the window handle
    HWND GetWindowHandle() const;
    /// callback function that processes messages sent to the window
    static LRESULT WINAPI WindowCallback(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    /// private default constructor
    DXGIRenderWindow();

    HINSTANCE instance;
    HWND hwnd;
    ID3D11Device* device;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11Texture2D* depthStencilBuffer;
    ID3D11DepthStencilView* depthStencilView;
    bool fullScreen;
    bool depthBuffer;
    bool msaaEnable;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
