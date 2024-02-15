//------------------------------------------------------------------------------
//  renderqueue.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "renderqueue.h"
#include "debug.h"
#include "chrisslyarray.h"
#include <stdint.h>

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
RenderQueue::RenderQueue() :
    renderablePasses(NULL)
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
    this->renderablePasses = NULL;
    ce_array_init(this->renderablePasses, capacity);
}

 //------------------------------------------------------------------------------
/**
*/
void
RenderQueue::Destroy()
{
    ce_array_delete(this->renderablePasses);
}

//------------------------------------------------------------------------------
/**
*/
void
RenderQueue::Clear()
{
    if (this->renderablePasses) ce_array_header(this->renderablePasses)->size = 0U;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderQueue::AddRenderable(SubEntity* const rend, Pass* const pass)
{
    if (!ce_array_full(this->renderablePasses))
    {
        RenderablePass* renderablePass = this->renderablePasses + ce_array_header(this->renderablePasses)->size;
        renderablePass->renderable = rend;
        renderablePass->pass = pass;

        ce_array_header(this->renderablePasses)->size++;
    }
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
RenderQueue::GetNumRenderablePasses() const
{
    return (unsigned short)ce_array_size(this->renderablePasses);
}

//------------------------------------------------------------------------------
/**
*/
RenderablePass*
RenderQueue::GetRenderablePass(unsigned short index) const
{
    return this->renderablePasses + (uintptr_t)index;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderQueue::Sort()
{
    int swapIndex, searchIndex, hitIndex, numRenderablePasses = ce_array_size(this->renderablePasses);
    for (swapIndex = 0; swapIndex < numRenderablePasses - 1; ++swapIndex)
    {
        hitIndex = swapIndex;
        for (searchIndex = swapIndex + 1; searchIndex < numRenderablePasses; ++searchIndex)
        {
            RenderablePass* rpHit = this->renderablePasses + (uintptr_t)hitIndex;
            RenderablePass* rpSearch = this->renderablePasses + (uintptr_t)searchIndex;
            if (rpSearch->renderable->distanceToCamera > rpHit->renderable->distanceToCamera)
            {
                hitIndex = searchIndex;
            }
        }

        if (hitIndex != swapIndex)
        {
            RenderablePass rpTmp = *(this->renderablePasses + (uintptr_t)swapIndex);
            *(this->renderablePasses + (uintptr_t)swapIndex) = *(this->renderablePasses + (uintptr_t)hitIndex);
            *(this->renderablePasses + (uintptr_t)hitIndex) = rpTmp;
        }
    }
}

} // namespace graphics
} // namespace chrissly

