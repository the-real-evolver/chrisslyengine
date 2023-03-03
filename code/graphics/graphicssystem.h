#ifndef GRAPHICSSYSTEM_H_
#define GRAPHICSSYSTEM_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::GraphicsSystem

    (C) 2010 Christian Bleicher
*/
#include "gpuprogrammanager.h"
#include "meshmanager.h"
#include "texturemanager.h"
#include "scenemanager.h"
#include "materialmanager.h"
#include "renderwindow.h"
#include "rendersystem.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class GraphicsSystem
{
public:
    /// get pointer to the singleton
    static GraphicsSystem* const Instance()
    {
        return Singleton;
    }

    /// default constructor
    GraphicsSystem();
    /// destructor
    ~GraphicsSystem();
    /// initialise graphicssystem
    RenderWindow* const Initialise(void* const customParams = NULL);
    /// retrieves a pointer to the window that was created automatically
    RenderWindow* const GetAutoCreatedWindow() const;
    /// render one frame
    void RenderOneFrame();
    /// retrieve a pointer to the currently selected render system
    RenderSystem* const GetRenderSystem() const;
    /// gets the frame number
    unsigned int GetFrameNumber() const;
    /// if you run your own render loop call this once per frame to update animations
    void RaiseFrameNumber();

private:
    /// copy constructor
    GraphicsSystem(const GraphicsSystem&);
    /// prevent copy by assignment
    GraphicsSystem& operator = (const GraphicsSystem&);

    static GraphicsSystem* Singleton;

    RenderSystem* activeRenderer;
    GpuProgramManager* gpuProgramManager;
    MeshManager* meshManager;
    TextureManager* textureManager;
    MaterialManager* materialManager;
    SceneManager* sceneManager;
    RenderWindow* autoWindow;
    RenderTarget** renderTargets;
    unsigned int frameNumber;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
