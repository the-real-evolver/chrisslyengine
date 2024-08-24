#ifndef GLES2RENDERSYSTEM_H_
#define GLES2RENDERSYSTEM_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::GLES2RenderSystem

    (C) 2012 Christian Bleicher
*/
#include "renderwindow.h"
#include "subentity.h"
#include "hashtable.h"
#include "gles2gpuprogram.h"
#include <GLES2/gl2.h>

//------------------------------------------------------------------------------
namespace chrissly
{
class GLES2RenderSystem
{
public:
    /// get pointer to the singleton
    static GLES2RenderSystem* const Instance()
    {
        return Singleton;
    }

    /// default constructor
    GLES2RenderSystem();
    /// destructor
    ~GLES2RenderSystem();

    /// start up the renderer
    graphics::RenderWindow* Initialise(graphics::ConfigOptions* const config);
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
    void SetTextureMatrix(const core::Matrix4& xform);
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

    /// gets the default light shader parameters (useful if a custom shader wants to use the default lighting)
    const core::Matrix4* GetDefaultLightShaderParams() const;
    /// get pointer to the default shadow caster gpu program
    GLES2GpuProgram* GetDefaultShadowCasterGpuProgram() const;
    /// get pointer to the default transparent shadow caster alpha test gpu program
    GLES2GpuProgram* GetDefaultTransparentShadowCasterAlphaTestGpuProgram() const;
    /// get pointer to the default shadow receiver gpu program
    GLES2GpuProgram* GetDefaultShadowReceiverGpuProgram() const;
    /// get pointer to the default shadow caster skeletal anim gpu program
    GLES2GpuProgram* GetDefaultShadowCasterSkeletalAnimGpuProgram() const;

private:
    /// copy constructor
    GLES2RenderSystem(const GLES2RenderSystem&);
    /// prevent copy by assignment
    GLES2RenderSystem& operator = (const GLES2RenderSystem&);

    static GLES2RenderSystem* Singleton;

    graphics::Viewport* activeViewport;

    unsigned int ambientLight;
    core::Matrix4 defaultLightShaderParams[4];

    core::Matrix4 worldMatrix;
    core::Matrix4 viewMatrix;
    core::Matrix4 projectionMatrix;

    GLES2GpuProgram* defaultGpuProgram;
    GLES2GpuProgram* defaultGpuProgramFog;
    GLES2GpuProgram* defaultGpuProgramLit;
    GLES2GpuProgram* defaultGpuProgramLitFog;
    GLES2GpuProgram* defaultGpuProgramMorphAnim;
    GLES2GpuProgram* defaultGpuProgramSkeletalAnim;
    GLES2GpuProgram* defaultGpuProgramShadowCaster;
    GLES2GpuProgram* defaultGpuProgramTransparentShadowCasterAlphaTest;
    GLES2GpuProgram* defaultGpuProgramShadowReceiver;
    GLES2GpuProgram* defaultGpuProgramShadowCasterSkeletalAnim;
    GLES2GpuProgram* currentGpuProgram;

    GLfloat glWorldMatrix[16];
    GLfloat glViewMatrix[16];
    GLfloat glProjectionMatrix[16];
    GLfloat glTextureMatrix[16];
    GLfloat glWorldViewProjectionMatrix[16];

    GLint numTextureUnits;
    GLint maxVertexAttribs;

    bool msaaEnable;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
