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
    graphics::RenderWindow* Initialise(void* const customParams);
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
    /// get the current active viewport
    graphics::Viewport* GetViewport() const;
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

    /// get pointer to the default shadow caster gpu program
    D3D11GpuProgram* GetDefaultShadowCasterGpuProgram() const;
    /// get pointer to the default transparent shadow caster gpu program
    D3D11GpuProgram* GetDefaultTransparentShadowCasterGpuProgram() const;
    /// get pointer to the default transparent shadow caster alpha test gpu program
    D3D11GpuProgram* GetDefaultTransparentShadowCasterAlphaTestGpuProgram() const;
    /// get pointer to the default shadow caster morph anim gpu program
    D3D11GpuProgram* GetDefaultShadowCasterMorphAnimGpuProgram() const;
    /// get pointer to the default transparent shadow caster morph anim gpu program
    D3D11GpuProgram* GetDefaultTransparentShadowCasterMorphAnimGpuProgram() const;
    /// get pointer to the default shadow caster skeletal anim gpu program
    D3D11GpuProgram* GetDefaultShadowCasterSkeletalAnimGpuProgram() const;
    /// get pointer to the default shadow receiver gpu program
    D3D11GpuProgram* GetDefaultShadowReceiverGpuProgram() const;

    /// get pointer to the d3d11 device
    ID3D11Device* GetDevice() const;
    /// get pointer to the d3d11 device context
    ID3D11DeviceContext* GetContext() const;
    /// create depth buffer with the given dimensions (format: DXGI_FORMAT_D24_UNORM_S8_UINT)
    void CreateDepthBuffer(UINT width, UINT height, bool msaa, ID3D11Texture2D** bufferOut, ID3D11DepthStencilView** viewOut);

private:
    /// copy constructor
    D3D11RenderSystem(const D3D11RenderSystem&);
    /// prevent copy by assignment
    D3D11RenderSystem& operator = (const D3D11RenderSystem&);

    static D3D11RenderSystem* Singleton;

    graphics::Viewport* activeViewport;

    unsigned int ambientLight;
    core::Matrix4 defaultLightShaderParams[4U];

    core::Matrix4 worldMatrix;
    core::Matrix4 viewMatrix;
    core::Matrix4 projectionMatrix;
    core::Matrix4 textureMatrix;

    D3D11GpuProgram* defaultGpuProgram;
    D3D11GpuProgram* defaultGpuProgramFog;
    D3D11GpuProgram* defaultGpuProgramLit;
    D3D11GpuProgram* defaultGpuProgramLitFog;
    D3D11GpuProgram* defaultGpuProgramMorphAnim;
    D3D11GpuProgram* defaultGpuProgramSkeletalAnim;
    D3D11GpuProgram* defaultGpuProgramNoTexture;
    D3D11GpuProgram* defaultGpuProgramFogNoTexture;
    D3D11GpuProgram* defaultGpuProgramLitNoTexture;
    D3D11GpuProgram* defaultGpuProgramLitFogNoTexture;
    D3D11GpuProgram* defaultGpuProgramMorphAnimNoTexture;
    D3D11GpuProgram* defaultGpuProgramSkeletalAnimNoTexture;
    D3D11GpuProgram* defaultGpuProgramShadowCaster;
    D3D11GpuProgram* defaultGpuProgramTransparentShadowCaster;
    D3D11GpuProgram* defaultGpuProgramTransparentShadowCasterAlphaTest;
    D3D11GpuProgram* defaultGpuProgramShadowReceiver;
    D3D11GpuProgram* defaultGpuProgramShadowCasterMorphAnim;
    D3D11GpuProgram* defaultGpuProgramTransparentShadowCasterMorphAnim;
    D3D11GpuProgram* defaultGpuProgramShadowCasterSkeletalAnim;
    D3D11GpuProgram* currentGpuProgram;

    ID3D11Device* device;
    ID3D11DeviceContext* context;
    D3D11_VIEWPORT viewPort;
    D3D11_RECT scissorRect[1U];
    ID3D11InputLayout* inputLayout;
    ID3D11InputLayout* inputLayoutMorphAnim;
    ID3D11InputLayout* inputLayoutSkeletalAnim;

    D3D11_RASTERIZER_DESC currentRasterState;
    D3D11_DEPTH_STENCIL_DESC currentDepthStencilState;
    D3D11_BLEND_DESC currentBlendState;
    D3D11_SAMPLER_DESC currentSamplerState;
    bool msaaEnable;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
