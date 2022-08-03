//------------------------------------------------------------------------------
//  graphicssystem.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "graphicssystem.h"
#include "memoryallocatorconfig.h"

namespace chrissly
{
namespace graphics
{

GraphicsSystem* GraphicsSystem::Singleton = NULL;

//------------------------------------------------------------------------------
/**
*/
GraphicsSystem::GraphicsSystem() :
    autoWindow(NULL),
    renderTargets(NULL)
{
    Singleton = this;
    this->activeRenderer = CE_NEW RenderSystem();
    this->gpuProgramManager = CE_NEW GpuProgramManager();
    this->meshManager = CE_NEW MeshManager();
    this->textureManager = CE_NEW TextureManager();
    this->materialManager = CE_NEW MaterialManager();
    this->sceneManager = CE_NEW SceneManager();
    ce_array_init(this->renderTargets, 1U);
}

//------------------------------------------------------------------------------
/**
*/
GraphicsSystem::~GraphicsSystem()
{
    Singleton = NULL;

    unsigned int i;
    for (i = 0U; i < ce_array_size(this->renderTargets); ++i)
    {
        CE_DELETE this->renderTargets[i];
    }
    ce_array_delete(this->renderTargets);

    CE_DELETE this->sceneManager;
    CE_DELETE this->materialManager;
    CE_DELETE this->textureManager;
    CE_DELETE this->meshManager;
    CE_DELETE this->gpuProgramManager;

    this->activeRenderer->Shutdown();
    CE_DELETE this->activeRenderer;
}

//------------------------------------------------------------------------------
/**
*/
RenderWindow* const
GraphicsSystem::Initialise(void* const customParams)
{
    this->autoWindow = this->activeRenderer->Initialise(customParams);

    ce_array_push_back(this->renderTargets, this->autoWindow);

    return this->autoWindow;
}

//------------------------------------------------------------------------------
/**
*/
RenderWindow* const
GraphicsSystem::GetAutoCreatedWindow() const
{
    return this->autoWindow;
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsSystem::RenderOneFrame()
{
    // update all rendertargets
    unsigned int index;
    for (index = 0U; index < ce_array_size(this->renderTargets); ++index)
    {
        RenderTarget* rt = this->renderTargets[index];
        rt->Update();
        rt->SwapBuffers();
    }
}

//------------------------------------------------------------------------------
/**
*/
RenderSystem* const
GraphicsSystem::GetRenderSystem() const
{
    return this->activeRenderer;
}

} // namespace graphics
} // namespace chrissly
