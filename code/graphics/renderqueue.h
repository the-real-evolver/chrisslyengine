#ifndef RENDERQUEUE_H_
#define RENDERQUEUE_H_
//------------------------------------------------------------------------------
/**
    @struct chrissly::graphics::RenderablePass

    @class chrissly::graphics::RenderQueue

    (C) 2011 Christian Bleicher
*/
#include "subentity.h"
#include "pass.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

struct RenderablePass
{
    /// pointer to the Renderable details
    SubEntity* renderable;
    /// pointer to the Pass
    Pass* pass;
};

class RenderQueue
{
public:
    /// default constructor
    RenderQueue();
    /// destructor
    ~RenderQueue();
    /// allocate queue
    void Initialise(unsigned short capacity);
    /// destroy queue
    void Destroy();
    /// empty the queue
    void Clear();
    /// add a renderable to the queue using a given pass
    void AddRenderable(SubEntity* const rend, Pass* const pass);
    /// sort insert renderable in order of distance to camera, from furthest to nearest
    void InsertSortedRenderable(SubEntity* const rend, Pass* const pass);
    /// gets the number of renderable pass currently populating the queue
    unsigned short GetNumRenderablePasses() const;
    /// get RenderablePass at given index
    RenderablePass* const GetRenderablePass(unsigned short index) const;

private:
    RenderablePass* renderablePasses;
    unsigned short renderablePassesCapacity;
    unsigned short numRenderablePasses;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif