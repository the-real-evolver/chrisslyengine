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
    this->format = graphics::PF_R8G8B8A8;
    this->width = 512;

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

    sceGuScissor(0, 0, width, height);
    sceGuDisable(GU_SCISSOR_TEST);
    sceGuEnable(GU_CLIP_PLANES);
    sceGuDepthRange(65535, 0);
    sceGuDepthFunc(GU_GEQUAL);
    sceGuShadeModel(GU_SMOOTH);

    sceGuFinish();
    sceGuSync(0, 0);
    sceGuDisplay(GU_TRUE);
}
    
//------------------------------------------------------------------------------
/**
*/
void
PSPRenderWindow::Update()
{
    sceGuStart(GU_DIRECT, PSPRenderSystem::Instance()->GetDisplayList());

    // update all viewports
    unsigned int index;
    for (index = 0; index < this->numViewports; index++)
    {
        graphics::Viewport* vp = (graphics::Viewport*)DynamicArrayGet(&this->viewportList, index);
        vp->Update();
    }
 
    sceGuFinish();
    sceGuSync(0, 0);
    
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