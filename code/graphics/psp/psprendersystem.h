#ifndef PSPRenderSystem_H_
#define PSPRenderSystem_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::PSPRenderSystem

    (C) 2012 Christian Bleicher
*/
#include "renderwindow.h"
#include "subentity.h"
#include <pspgu.h>

//------------------------------------------------------------------------------
namespace chrissly
{
class PSPRenderSystem
{
public:
    /// get pointer to the singleton
    static PSPRenderSystem* Instance()
    {
        return Singleton;
    }

    /// default constructor
    PSPRenderSystem();
    /// destructor
    ~PSPRenderSystem();
    
    /// start up the renderer
    graphics::RenderWindow* _Initialise(void* customParams);
    /// shutdown the renderer and cleanup resources
    void Shutdown();
    /// set current render target to target, enabling its device context if needed
    void _SetRenderTarget(graphics::RenderTarget *target);
    /**
        Sets the provided viewport as the active one for future
        rendering operations. This viewport is aware of it's own
        camera and render target.
    */
    void _SetViewport(graphics::Viewport *vp);
    /// sets the world transform matrix
    void _SetWorldMatrix(const core::Matrix4& m);
    /// Sets the view transform matrix
    void _SetViewMatrix(const core::Matrix4& m);
    /// Sets the projection transform matrix
    void _SetProjectionMatrix(const core::Matrix4& m);
    /// sets the texture coordinate transformation matrix for the texture unit
    void _SetTextureMatrix(const core::Matrix4& xform);
    /// render something to the active viewport
    /**
        Low-level rendering interface to perform rendering
        operations. Unlikely to be used directly by client
        applications, since the SceneManager and various support
        classes will be responsible for calling this method.
    */
    void _Render(graphics::SubEntity* renderable);
    /// start of rendering on a single viewport
    void _BeginFrame();
    /// ends rendering of a frame to the current viewport
    void _EndFrame();
    
    /// method for setting up the renderstate for a rendering pass
    void _SetPass(graphics::Pass* pass);
    /// sets the colour & strength of the ambient (global directionless) light in the world
    void SetAmbientLight(unsigned int colour);

    /// returns displaylist
    void* GetDisplayList() const;
    /// notify the rendersystem that a morphkeyframe was build
    void _NotifyMorphKeyFrameBuild();

private:
    /// copy constructor
    PSPRenderSystem(const PSPRenderSystem&cc) {}; 

    static PSPRenderSystem* Singleton;
    
    /// the GE store the commands for processing in this array
    static unsigned int __attribute__((aligned(16))) DisplayList[262144];
    
    unsigned int ambientLight;
    ScePspFMatrix4 viewMat;
    ScePspFMatrix4 projMat;
    ScePspFMatrix4 worldMat;
    ScePspFMatrix4 texMat;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
