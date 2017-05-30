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
    autoWindow(NULL)
{
    Singleton = this;
    this->activeRenderer = CE_NEW RenderSystem();
    this->meshManager = CE_NEW MeshManager();
    this->textureManager = CE_NEW TextureManager();
    this->materialManager = CE_NEW MaterialManager();
    this->sceneManager = CE_NEW SceneManager();
    ce_dynamic_array_init(&this->renderTargets, 1U);
}

//------------------------------------------------------------------------------
/**
*/
GraphicsSystem::~GraphicsSystem()
{
    Singleton = NULL;

    unsigned int i;
    for (i = 0U; i < this->renderTargets.size; ++i)
    {
        CE_DELETE (RenderTarget*)ce_dynamic_array_get(&this->renderTargets, i);
    }
    ce_dynamic_array_delete(&this->renderTargets);

    CE_DELETE this->sceneManager;
    CE_DELETE this->materialManager;
    CE_DELETE this->textureManager;
    CE_DELETE this->meshManager;

    this->activeRenderer->Shutdown();
    CE_DELETE this->activeRenderer;
}

//------------------------------------------------------------------------------
/**
*/
RenderWindow*
GraphicsSystem::Initialise(void* customParams)
{
    this->autoWindow = this->activeRenderer->_Initialise(customParams);

    ce_dynamic_array_push_back(&this->renderTargets, this->autoWindow);

    return this->autoWindow;
}

//------------------------------------------------------------------------------
/**
*/
RenderWindow*
GraphicsSystem::GetAutoCreatedWindow()
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
    for (index = 0U; index < this->renderTargets.size; ++index)
    {
        RenderTarget* rt = (RenderTarget*)ce_dynamic_array_get(&this->renderTargets, index);
        rt->Update();
        rt->SwapBuffers();
    }
}

//------------------------------------------------------------------------------
/**
*/
RenderSystem*
GraphicsSystem::GetRenderSystem()
{
    return this->activeRenderer;
}

} // namespace graphics
} // namespace chrissly
