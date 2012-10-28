#ifndef KEYFRAME_H_
#define KEYFRAME_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::VertexMorphKeyFrame

    (C) 2012 Christian Bleicher
*/
#include "vertexdata.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class VertexMorphKeyFrame
{
public:
    /// constructor with keytime
    VertexMorphKeyFrame(float time);
    /// destructor
    ~VertexMorphKeyFrame();

    /// gets the time of this keyframe in the animation sequence
    float GetTime() const;

    /// vertex data
    VertexData* vertexData;

private:
    float time;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
