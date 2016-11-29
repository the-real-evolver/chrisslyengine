//------------------------------------------------------------------------------
//  d3d11rendertexture.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "d3d11rendertexture.h"

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
D3D11RenderTexture::D3D11RenderTexture()
{

}

//------------------------------------------------------------------------------
/**
*/
D3D11RenderTexture::~D3D11RenderTexture()
{
    this->RemoveAllViewports();
}

//------------------------------------------------------------------------------
/**
*/
void
D3D11RenderTexture::SwapBuffers()
{
    // do nothing no need for doublebuffering
}

} // namespace chrissly