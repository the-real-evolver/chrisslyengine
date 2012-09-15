//------------------------------------------------------------------------------
//  psprenderwindow.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "psprenderwindow.h"
#include "psprendersystem.h"
#include "psphardwarebuffermanager.h"
#include <pspgu.h>
#include <pspdisplay.h>

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
PSPRenderWindow::PSPRenderWindow()
{

}

//------------------------------------------------------------------------------
/**
*/
PSPRenderWindow::~PSPRenderWindow()
{
    this->RemoveAllViewports();
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderWindow::Create()
{
    const unsigned int bufferWidth = 512;
    const unsigned int width = 480;
    const unsigned int height = 272;

    void* drawBuffer = PSPHardwareBufferManager::GetStaticVramBuffer(bufferWidth, height, GU_PSM_8888);
    void* dispBuffer = PSPHardwareBufferManager::GetStaticVramBuffer(bufferWidth, height, GU_PSM_8888);
    void* depthBuffer = PSPHardwareBufferManager::GetStaticVramBuffer(bufferWidth, height, GU_PSM_4444);
    
    sceGuStart(GU_DIRECT, PSPRenderSystem::Instance()->GetDisplayList());
    sceGuDrawBuffer(GU_PSM_8888, drawBuffer, bufferWidth);
    sceGuDispBuffer(width, height, dispBuffer, bufferWidth);
    sceGuDepthBuffer(depthBuffer, bufferWidth);
    sceGuFinish();
    sceGuSync(0, 0);
    
    this->format = graphics::PF_R8G8B8A8;
    this->width = bufferWidth;
    this->height = height;
}
    
//------------------------------------------------------------------------------
/**
*/
void
PSPRenderWindow::Update()
{
    // update all viewports
    unsigned int index;
    for (index = 0; index < this->numViewports; index++)
    {
        graphics::Viewport* vp = (graphics::Viewport*)DynamicArrayGet(&this->viewportList, index);
        vp->Update();
    }

    // build scheduled morphkeyframes here
}

//------------------------------------------------------------------------------
/**
*/
void
PSPRenderWindow::SwapBuffers()
{
    sceDisplayWaitVblankStart();
    this->buffer = sceGuSwapBuffers();
}

} // namespace chrissly