//------------------------------------------------------------------------------
//  renderqueuesortinggrouping.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "renderqueuesortinggrouping.h"
#include "debug.h"

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
QueuedRenderableCollection::QueuedRenderableCollection() : numRenderablePasses(0)
{

}

//------------------------------------------------------------------------------
/**
*/
QueuedRenderableCollection::~QueuedRenderableCollection()
{
    this->Destroy();
}

//------------------------------------------------------------------------------
/**
*/
void
QueuedRenderableCollection::Initialise(unsigned short capacity)
{
    DynamicArrayInit(&this->renderablePasses, capacity);

    unsigned int i;
    for (i = 0; i < this->renderablePasses.cur_size; i++)
    {
        RenderablePass* renderablePass = CE_NEW RenderablePass(NULL, NULL);

        if (!DynamicArraySet(&this->renderablePasses, i, renderablePass))
        {
            CE_ASSERT(false, "QueuedRenderableCollection::Initialise(): DynamicArraySet failed \n");
        }
    }

    this->numRenderablePasses = 0;
}

 //------------------------------------------------------------------------------
/**
*/
void
QueuedRenderableCollection::Destroy()
{
    unsigned int i;
    for (i = 0; i < this->renderablePasses.cur_size; i++)
    {
        CE_DELETE (RenderablePass*)DynamicArrayGet(&this->renderablePasses, i);
    }

    DynamicArrayDelete(&this->renderablePasses);

    this->numRenderablePasses = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
QueuedRenderableCollection::Clear()
{
    this->numRenderablePasses = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
QueuedRenderableCollection::AddRenderable(SubEntity* rend, Pass* pass)
{
    RenderablePass* renderablePass = (RenderablePass*)DynamicArrayGet(&this->renderablePasses, this->numRenderablePasses);
    renderablePass->renderable = rend;
    renderablePass->pass = pass;

    this->numRenderablePasses++;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
QueuedRenderableCollection::GetNumRenderablePasses() const
{
    return this->numRenderablePasses;
}

//------------------------------------------------------------------------------
/**
*/
RenderablePass*
QueuedRenderableCollection::GetRenderablePass(unsigned short index) const
{
    return (RenderablePass*)DynamicArrayGet(&this->renderablePasses, index);
}

} // namespace graphics
} // namespace chrissly

