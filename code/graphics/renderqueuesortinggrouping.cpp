//------------------------------------------------------------------------------
//  renderqueuesortinggrouping.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "renderqueuesortinggrouping.h"

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
QueuedRenderableCollection::QueuedRenderableCollection() :
    numRenderablePasses(0)
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
    ce_dynamic_array_init(&this->renderablePasses, capacity);

    unsigned int i;
    for (i = 0; i < this->renderablePasses.capacity; ++i)
    {
        RenderablePass* renderablePass = CE_NEW RenderablePass(NULL, NULL);
        ce_dynamic_array_set(&this->renderablePasses, i, renderablePass);
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
    for (i = 0; i < this->renderablePasses.capacity; ++i)
    {
        CE_DELETE (RenderablePass*)ce_dynamic_array_get(&this->renderablePasses, i);
    }

    ce_dynamic_array_delete(&this->renderablePasses);

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
    RenderablePass* renderablePass = (RenderablePass*)ce_dynamic_array_get(&this->renderablePasses, this->numRenderablePasses);
    renderablePass->renderable = rend;
    renderablePass->pass = pass;

    ++this->numRenderablePasses;
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
    return (RenderablePass*)ce_dynamic_array_get(&this->renderablePasses, index);
}

} // namespace graphics
} // namespace chrissly

