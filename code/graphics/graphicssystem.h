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

// this struct has to be passed to GraphicsSystem::Initialise() in order to initialise the RenderWindow and RenderSystem
struct ConfigOptions
{
    /// default constructor
    ConfigOptions() : instance(NULL), windowWidth(0), windowHeight(0), fullScreen(false), depthBuffer(true), msaaEnable(false) {};
    /// constructor with all parameters
    ConfigOptions(void* inst, int width, int height, bool fs, bool depth = true, bool msaa = false) : instance(inst), windowWidth(width), windowHeight(height), fullScreen(fs), depthBuffer(depth), msaaEnable(msaa) {};

    void* instance;
    int windowWidth;
    int windowHeight;
    bool fullScreen;
    bool depthBuffer;
    bool msaaEnable;
};

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
    RenderWindow* Initialise(void* const customParams = NULL);
    /// retrieves a pointer to the window that was created automatically
    RenderWindow* GetAutoCreatedWindow() const;
    /// render one frame
    void RenderOneFrame();
    /// retrieve a pointer to the currently selected render system
    RenderSystem* GetRenderSystem() const;
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
