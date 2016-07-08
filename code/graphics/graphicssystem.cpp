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
    this->sceneManager = CE_NEW SceneManager();
    this->materialManager = CE_NEW MaterialManager();
    DynamicArrayInit(&this->renderTargets, 1);
}

//------------------------------------------------------------------------------
/**
*/
GraphicsSystem::~GraphicsSystem()
{
    Singleton = NULL;
    this->activeRenderer->Shutdown();
    CE_DELETE this->activeRenderer;
    CE_DELETE this->meshManager;
    CE_DELETE this->textureManager;
    CE_DELETE this->sceneManager;
    CE_DELETE this->materialManager;

    unsigned int i;
    for (i = 0; i < this->renderTargets.size; ++i)
    {
        CE_DELETE (RenderTarget*)DynamicArrayGet(&this->renderTargets, i);
    }
    DynamicArrayDelete(&this->renderTargets);
}

//------------------------------------------------------------------------------
/**
*/
RenderWindow*
GraphicsSystem::Initialise(void* customParams)
{
    this->autoWindow = this->activeRenderer->_Initialise(customParams);

    DynamicArrayPushBack(&this->renderTargets, this->autoWindow);

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
    for (index = 0; index < this->renderTargets.size; ++index)
    {
        RenderTarget* rt = (RenderTarget*)DynamicArrayGet(&this->renderTargets, index);
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
