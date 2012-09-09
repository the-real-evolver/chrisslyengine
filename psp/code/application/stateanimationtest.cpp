//------------------------------------------------------------------------------
//  stateanimationtest.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "stateanimationtest.h"
#include "statematerialtest.h"
#include "statemanager.h"
#include "graphicssystem.h"
#include "rendersystem.h"
#include "debug.h"
#include <pspgu.h>
#include <pspgum.h>
#include <pspdisplay.h>
#include <psputils.h>

namespace application
{

using namespace chrissly::graphics;
using namespace chrissly;

StateAnimationTest* StateAnimationTest::Singleton = NULL;

//------------------------------------------------------------------------------
/**
*/
StateAnimationTest::StateAnimationTest()
{
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
StateAnimationTest::~StateAnimationTest()
{
    Singleton = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
StateAnimationTest::Enter()
{
    sceCtrlSetSamplingCycle(100);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    this->tex0 = TextureManager::Instance()->Load("cerberus.tex");
    this->tex0->SetFormat(PF_R5G6B5);
    this->tex0->SetWidth(512);
    this->tex0->SetHeight(512);

    this->material = new Material();
    
    this->pass0 = this->material->CreatePass();
    TextureUnitState* tus = this->pass0->CreateTextureUnitState();
    tus->SetTexture(this->tex0);

    this->animation = MeshManager::Instance()->Load("cerberus_walk.mesh");

    this->sourceMorphWeight = 1.0f;
    this->targetMorphWeight = 0.0f;
    this->meshMorphBuffer = CE_MALLOC(1024 * 256);
    this->currentKeyFrame = 0;
    this->frame = 0;
    this->animSpeed = 6;
    
    this->ry = 0.0f;
    
    // build interleaved buffer
    RenderSystem::Instance()->FillInterleaved(this->animation->GetSubMesh(this->currentKeyFrame)->_vertexBuffer,
                                               this->animation->GetSubMesh(this->currentKeyFrame + 1)->_vertexBuffer,
                                               this->meshMorphBuffer,
                                               36,
                                               this->animation->GetSubMesh(this->currentKeyFrame)->_vertexCount * 2 * 36);

    sceKernelDcacheWritebackAll();
}

//------------------------------------------------------------------------------
/**
*/
void
StateAnimationTest::Exit()
{	
    this->material->RemoveAllPasses();
    delete this->material;
  
    MeshManager::Instance()->RemoveAll();   
    TextureManager::Instance()->RemoveAll();
    
    CE_FREE(this->meshMorphBuffer);
}

//------------------------------------------------------------------------------
/**
*/
void
StateAnimationTest::Trigger()
{
    sceGuStart(GU_DIRECT, PSPRenderSystem::Instance()->GetDisplayList());    
    sceGuDrawBufferList(GU_PSM_8888, this->frameBuffer, 512);
    sceGuOffset(2048 - (480 / 2), 2048 - (272 / 2));
    sceGuViewport(2048, 2048, 480, 272);
    sceGuClearColor(0xff554433);
    sceGuClearDepth(0);
    sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
    sceGuScissor(0, 0, 480, 272);
    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumPerspective(75.0f, 16.0f / 9.0f, 0.5f, 1000.0f);
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();

    ScePspFMatrix4 world;
    ScePspFVector3 pos = {0.0f, 0.0f, -4.0f};
    ScePspFVector3 rot = {0.0f, 1.0f * this->ry * (GU_PI / 180.0f), 0.0f};
    ScePspFVector3 scale = {5.0f, 5.0f, 5.0f};
    gumLoadIdentity(&world);
    gumTranslate(&world, &pos);
    gumRotateXYZ(&world, &rot);
    gumScale(&world, &scale);
    sceGuSetMatrix(GU_MODEL, &world);
    
    SceneManager::Instance()->_SetPass(this->pass0);
    
    this->sourceMorphWeight = 1.0f - ((1.0f / this->animSpeed) * this->frame);
    this->targetMorphWeight = (1.0f / this->animSpeed)  * this->frame;
    sceGuMorphWeight(0, this->sourceMorphWeight);
    sceGuMorphWeight(1, this->targetMorphWeight);
    
    sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D | GU_VERTICES(2) | GU_WEIGHTS(2), 
                    this->animation->GetSubMesh(this->currentKeyFrame)->_vertexCount, 0, 
                    this->meshMorphBuffer);

    // render end
    sceGuFinish();
    sceGuSync(0, 0);       

    this->frame++;
    if (this->frame == this->animSpeed)
    {
        // build interleaved buffer
        // Note: it is important to do that between sceGuSync and sceDisplayWaitVblankStart
        // and to flush the cache 
        RenderSystem::Instance()->FillInterleaved(this->animation->GetSubMesh(this->currentKeyFrame)->_vertexBuffer,
                                           this->animation->GetSubMesh(this->currentKeyFrame + 1)->_vertexBuffer,
                                           this->meshMorphBuffer,
                                           36,
                                           this->animation->GetSubMesh(this->currentKeyFrame)->_vertexCount * 2 * 36);
        sceKernelDcacheWritebackAll();
        
        this->currentKeyFrame++;
        this->frame = 0;
    }

    sceDisplayWaitVblankStart();
    this->frameBuffer = sceGuSwapBuffers();
    
    if (this->currentKeyFrame == (unsigned int)(this->animation->GetNumSubMeshes() - 1))
    {
        this->currentKeyFrame = 0;
    }
    
    // controls
    sceCtrlReadBufferPositive(&this->pad, 1);
    float tx = (this->pad.Lx - 128) / 127.0f;
    if (tx > 0.2f)
    {
        tx -= 0.2f;
    }
    else if (tx < -0.2f)
    {
        tx += 0.2f;
    }
    else
    {
        tx = 0.0f;
    }
    this->ry += 2.0f * tx;
    
    if (this->pad.Buttons & PSP_CTRL_CROSS)
    {
        StateManager::Instance()->ChangeState(StateMaterialTest::Instance());
    }
}

} // namespace application


