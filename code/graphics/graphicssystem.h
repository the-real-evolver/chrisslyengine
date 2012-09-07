#ifndef GRAPHICSSYSTEM_H_
#define GRAPHICSSYSTEM_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::GraphicsSystem

    (C) 2010 Christian Bleicher
*/
#include "dynamicarray.h"
#include "meshmanager.h"
#include "texturemanager.h"
#include "scenemanager.h"
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
    static GraphicsSystem* Instance()
    {
        return Singleton;
    }

    /// default constructor
    GraphicsSystem();
    /// destructor
    ~GraphicsSystem();
    /// initialise graphicssystem
    RenderWindow* Initialise(void* customParams = NULL);
    /// retrieves a pointer to the window that was created automatically
    RenderWindow* GetAutoCreatedWindow();
    /// render one frame
    void RenderOneFrame();
    /// retrieve a pointer to the currently selected render system
    RenderSystem* GetRenderSystem();

private:
    /// copy constructor
    GraphicsSystem(const GraphicsSystem&cc) {}; 

    static GraphicsSystem* Singleton;

    RenderSystem* activeRenderer;
    MeshManager* meshManager;
    TextureManager* textureManager;
    SceneManager* sceneManager;
    RenderWindow* autoWindow;
    DynamicArray renderTargets;
    unsigned short numRenderTargets;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
