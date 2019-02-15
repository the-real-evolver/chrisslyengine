#ifndef PSPRENDERSYSTEM_H_
#define PSPRENDERSYSTEM_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::PSPRenderSystem

    (C) 2012 Christian Bleicher
*/
#include "renderwindow.h"
#include "subentity.h"
#include "hashtable.h"
#include <pspgu.h>

//------------------------------------------------------------------------------
namespace chrissly
{
class PSPRenderSystem
{
public:
    /// get pointer to the singleton
    static PSPRenderSystem* const Instance()
    {
        return Singleton;
    }

    /// default constructor
    PSPRenderSystem();
    /// destructor
    ~PSPRenderSystem();

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

    /// returns a pointer to the displaylist
    void* const GetDisplayList() const;

private:
    /// copy constructor
    PSPRenderSystem(const PSPRenderSystem&);
    /// prevent copy by assignment
    PSPRenderSystem& operator = (const PSPRenderSystem&);

    static PSPRenderSystem* Singleton;

    ce_hash_table* lights;
    bool restoreLights;
    unsigned int ambientLight;
    ScePspFVector3 lightPos;
    ScePspFVector3 lightDir;
    ScePspFMatrix4 viewMat;
    ScePspFMatrix4 projMat;
    ScePspFMatrix4 worldMat;
    ScePspFMatrix4 texMat;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
