#ifndef D3D11RENDERSYSTEM_H_
#define D3D11RENDERSYSTEM_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::D3D11RenderSystem

    (C) 2016 Christian Bleicher
*/
#include "renderwindow.h"
#include "subentity.h"
#include "hashtable.h"
#include "d3d11statecache.h"

//------------------------------------------------------------------------------
namespace chrissly
{
class D3D11RenderSystem
{
public:
    /// get pointer to the singleton
    static D3D11RenderSystem* const Instance()
    {
        return Singleton;
    }

    /// default constructor
    D3D11RenderSystem();
    /// destructor
    ~D3D11RenderSystem();

    /// start up the renderer
    graphics::RenderWindow* const Initialise(void* const customParams);
    /// shutdown the renderer and cleanup resources
    void Shutdown();
    /// set current render target to target, enabling its device context if needed
    void SetRenderTarget(graphics::RenderTarget* const target);
    /**
        Sets the provided viewport as the active one for future
        rendering operations. This viewport is aware of it's own
        camera and render target.
    */
    void SetViewport(graphics::Viewport* const vp);
    /// sets the world transform matrix
    void SetWorldMatrix(const core::Matrix4& m);
    /// sets the view transform matrix
    void SetViewMatrix(const core::Matrix4& m);
    /// sets the projection transform matrix
    void SetProjectionMatrix(const core::Matrix4& m);
    /// sets the texture coordinate transformation matrix for the texture unit
    void SetTextureMatrix(const core::Matrix4& m);
    /// render something to the active viewport
    /**
        Low-level rendering interface to perform rendering
        operations. Unlikely to be used directly by client
        applications, since the SceneManager and various support
        classes will be responsible for calling this method.
    */
    void Render(graphics::SubEntity* const renderable);
    /// start of rendering on a single viewport
    void BeginFrame();
    /// ends rendering of a frame to the current viewport
    void EndFrame();
    /// method for setting up the renderstate for a rendering pass
    void SetPass(graphics::Pass* const pass);
    /// tells the rendersystem to use the attached set of lights (and no others)
    void ProcessLights(ce_hash_table* const lights);
    /// sets the colour & strength of the ambient (global directionless) light in the world
    void SetAmbientLight(unsigned int colour);

    /// get pointer to the default morph animation gpu program (useful if a custom pass wants to use morph animation)
    D3D11GpuProgram* const GetDefaultMorphAnimationGpuProgram() const;
    /// get pointer to the d3d11 device
    ID3D11Device* const GetDevice() const;
    /// get pointer to the d3d11 device context
    ID3D11DeviceContext* const GetContext() const;

private:
    /// copy constructor
    D3D11RenderSystem(const D3D11RenderSystem&);
    /// prevent copy by assignment
    D3D11RenderSystem& operator = (const D3D11RenderSystem&);

    static D3D11RenderSystem* Singleton;

    unsigned int ambientLight;
    core::Matrix4 defaultLightShaderParams[4U];

    core::Matrix4 worldMatrix;
    core::Matrix4 viewMatrix;
    core::Matrix4 projectionMatrix;

    D3D11GpuProgram* defaultGpuProgram;
    D3D11GpuProgram* defaultGpuProgramFog;
    D3D11GpuProgram* defaultGpuProgramLit;
    D3D11GpuProgram* defaultGpuProgramLitFog;
    D3D11GpuProgram* defaultGpuProgramMorphAnim;
    D3D11GpuProgram* currentGpuProgram;

    ID3D11Device* device;
    ID3D11DeviceContext* context;
    D3D11_VIEWPORT viewPort;
    ID3D11InputLayout* inputLayout;
    ID3D11InputLayout* inputLayoutMorphAnim;

    D3D11StateCache stateCache;
    D3D11_RASTERIZER_DESC currentRasterState;
    D3D11_DEPTH_STENCIL_DESC currentDepthStencilState;
    D3D11_BLEND_DESC currentBlendState;
    D3D11_SAMPLER_DESC currentSamplerState;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif