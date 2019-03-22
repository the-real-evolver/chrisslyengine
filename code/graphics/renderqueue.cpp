//------------------------------------------------------------------------------
//  renderqueue.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "renderqueue.h"
#include "debug.h"
#include <stdint.h>

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
RenderQueue::RenderQueue() :
    renderablePasses(NULL),
    renderablePassesCapacity(0U),
    numRenderablePasses(0U)
{

}

//------------------------------------------------------------------------------
/**
*/
RenderQueue::~RenderQueue()
{
    this->Destroy();
}

//------------------------------------------------------------------------------
/**
*/
void
RenderQueue::Initialise(unsigned short capacity)
{
    unsigned int bufferSize = capacity * sizeof(RenderablePass);
    this->renderablePasses = (RenderablePass*)CE_MALLOC(bufferSize);
    CE_ASSERT(this->renderablePasses != NULL, "RenderQueue::Initialise(): failed to allocate '%i' bytes\n", bufferSize);
    memset(this->renderablePasses, 0, bufferSize);
    this->renderablePassesCapacity = capacity;
    this->numRenderablePasses = 0U;
}

 //------------------------------------------------------------------------------
/**
*/
void
RenderQueue::Destroy()
{
    CE_FREE(this->renderablePasses);
    this->renderablePassesCapacity = 0U;
    this->numRenderablePasses = 0U;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderQueue::Clear()
{
    this->numRenderablePasses = 0U;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderQueue::AddRenderable(SubEntity* const rend, Pass* const pass)
{
    if (this->numRenderablePasses < this->renderablePassesCapacity)
    {
        RenderablePass* renderablePass = this->renderablePasses + (uintptr_t)this->numRenderablePasses;
        renderablePass->renderable = rend;
        renderablePass->pass = pass;

        ++this->numRenderablePasses;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
RenderQueue::InsertSortedRenderable(SubEntity* const rend, Pass* const pass)
{
    if (this->numRenderablePasses < this->renderablePassesCapacity)
    {
        RenderablePass* renderablePass = NULL;
        int index, insertionIndex;
        for (insertionIndex = 0; insertionIndex < this->numRenderablePasses; ++insertionIndex)
        {
            renderablePass = this->renderablePasses + (uintptr_t)insertionIndex;
            if (rend->distanceToCamera > renderablePass->renderable->distanceToCamera)
            {
                break;
            }
        }

        for (index = this->numRenderablePasses - 1; index >= insertionIndex; --index)
        {
            *(this->renderablePasses + (uintptr_t)index + 1U) = *(this->renderablePasses + (uintptr_t)index);
        }

        renderablePass = this->renderablePasses + (uintptr_t)insertionIndex;
        renderablePass->renderable = rend;
        renderablePass->pass = pass;

        ++this->numRenderablePasses;
    }
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
RenderQueue::GetNumRenderablePasses() const
{
    return this->numRenderablePasses;
}

//------------------------------------------------------------------------------
/**
*/
RenderablePass* const
RenderQueue::GetRenderablePass(unsigned short index) const
{
    return this->renderablePasses + (uintptr_t)index;
}

} // namespace graphics
} // namespace chrissly

