#ifndef DXGIRENDERWINDOW_H_
#define DXGIRENDERWINDOW_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::DXGIRenderWindow

    (C) 2016 Christian Bleicher
*/
#include "rendertarget.h"
#include <windows.h>
#pragma warning(disable : 4005)
#include <d3dx11.h>

//------------------------------------------------------------------------------
namespace chrissly
{
class DXGIRenderWindow : public graphics::RenderTarget
{
public:
    /// constructor with instance handle and device
    DXGIRenderWindow(HINSTANCE inst, ID3D11Device* dev);
    /// destructor
    ~DXGIRenderWindow();
    /// creates & displays the new window
    void Create();
    /// tells the window to update it's contents
    void Update();
    /// swaps the frame buffers to display the next frame
    void SwapBuffers();
    /// gets a pointer to the rendertarget view of the swapchain's buffer
    ID3D11RenderTargetView* GetRenderTargetView() const;
    /// gets a pointer to the depth stencil view
    ID3D11DepthStencilView* GetDepthStencilView() const;

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
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
