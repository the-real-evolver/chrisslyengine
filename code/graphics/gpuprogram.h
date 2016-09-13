#ifndef GPUPROGRAM_H_
#define GPUPROGRAM_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::GpuProgram

    (C) 2012 Christian Bleicher
*/

//------------------------------------------------------------------------------
#if __PSP__
namespace chrissly
{
namespace graphics
{

class GpuProgram {};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __ANDROID__
#include "android/gles2gpuprogram.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::GLES2GpuProgram GpuProgram;

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#elif __WINDOWS__
#include "windows/d3d11gpuprogram.h"
namespace chrissly
{
namespace graphics
{

typedef chrissly::D3D11GpuProgram GpuProgram;

} // namespace graphics
} // namespace chrissly
  //------------------------------------------------------------------------------
#else
#error "GpuProgram class not implemented on this platform!"
#endif
#endif
