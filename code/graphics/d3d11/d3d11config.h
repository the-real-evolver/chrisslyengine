#ifndef D3D11CONFIG_H_
#define D3D11CONFIG_H_
//------------------------------------------------------------------------------
/**
    @file graphics/d3d11/d3d11config.h

    (C) 2019 Christian Bleicher
*/

#pragma warning(disable : 4005)
#if __CE_USE_LEGACY_DIRECTX_SDK__
#include <d3dx11.h>
#else
#include <d3d11.h>
#endif

#endif