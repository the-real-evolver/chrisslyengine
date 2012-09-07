#ifndef GLES2RENDERSYSTEM_H_
#define GLES2RENDERSYSTEM_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::GLES2RenderSystem

    (C) 2012 Christian Bleicher
*/
#include "renderwindow.h"
#include "subentity.h"
#include "gles2gpuprogram.h"
#include <GLES2/gl2.h>

//------------------------------------------------------------------------------
namespace chrissly
{
class GLES2RenderSystem
{
public:
    /// get pointer to the singleton
    static GLES2RenderSystem* Instance()
    {
        return Singleton;
    }

    /// default constructor
    GLES2RenderSystem();
    /// destructor
    ~GLES2RenderSystem();
    
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
    /// method for setting up the renderstate for a rendering pass
    void _SetPass(graphics::Pass* pass);
    /// sets the colour & strength of the ambient (global directionless) light in the world
    void SetAmbientLight(unsigned int colour);
    
    /// print some aspect of the current GL connection
    static void PrintGLString(const char *name, GLenum s);
    /// print value of the error flag the recent function call returned
    static void CheckGlError(const char* op);

private:
    /// copy constructor
    GLES2RenderSystem(const GLES2RenderSystem&cc) {};

    static GLES2RenderSystem* Singleton;

    unsigned int ambientLight;

    core::Matrix4 viewMatrix;
    core::Matrix4 projectionMatrix;

    static const char* FixedFunctionVertexShader;
    static const char* FixedFunctionFragmentShader;

    GLES2GpuProgram* gpuProgram;

    GLfloat glWorldMatrix[16];
    GLfloat glViewMatrix[16];
    GLfloat glProjectionMatrix[16];
    GLfloat glTextureMatrix[16];
    GLfloat glWorldViewProjectionMatrix[16];
    
    GLint numTextureUnits;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
