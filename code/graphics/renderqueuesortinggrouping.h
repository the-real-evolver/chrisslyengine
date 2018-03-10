#ifndef RENDERQUEUESORTINGGROUPING_H_
#define RENDERQUEUESORTINGGROUPING_H_
//------------------------------------------------------------------------------
/**
    @struct chrissly::graphics::RenderablePass

    @class chrissly::graphics::QueuedRenderableCollection

    (C) 2011 Christian Bleicher
*/
#include "subentity.h"
#include "pass.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

/**
    struct associating a single Pass with a single Renderable.
    This is used to for objects sorted by depth and thus not
    grouped by pass
*/
struct RenderablePass
{
    /// pointer to the Renderable details
    SubEntity* renderable;
    /// pointer to the Pass
    Pass* pass;

    RenderablePass(SubEntity* rend, Pass* p) : renderable(rend), pass(p) {}
};

/**
    collection of renderables
*/
class QueuedRenderableCollection
{
public:
    /// default constructor
    QueuedRenderableCollection();
    /// destructor
    ~QueuedRenderableCollection();
    /// allocate collection
    void Initialise(unsigned short capacity);
    /// destroy collection
    void Destroy();
    /// empty the collection
    void Clear();
    /// add a renderable to the collection using a given pass
    void AddRenderable(SubEntity* const rend, Pass* const pass);
    /// gets the number of renderable pass currently populating the collection
    unsigned short GetNumRenderablePasses() const;
    /// get RenderablePass at given index
    RenderablePass* const GetRenderablePass(unsigned short index) const;

private:
    mutable ce_dynamic_array renderablePasses;
    unsigned short numRenderablePasses;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif