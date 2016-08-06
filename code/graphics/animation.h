#ifndef ANIMATION_H_
#define ANIMATION_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::Animation

    (C) 2012 Christian Bleicher
*/
#include "animationtrack.h"
#include "dynamicarray.h"
#include "chrisslystring.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{
class Entity;

class Animation
{
public:
    /// constructor with name and length
    Animation(const char* name, float length);
    /// destructor
    ~Animation();

    /// gets the name of this animation
    const char* GetName() const;
    /// gets the total length of the animation
    float GetLength() const;
    /// creates a VertexAnimationTrack for animating vertex position data
    VertexAnimationTrack* CreateVertexTrack(unsigned char handle);
    /// gets the number of VertexAnimationTrack objects contained in this animation
    unsigned short GetNumVertexTracks() const;
    /// gets a Vertex track by it's index
    VertexAnimationTrack* GetVertexTrack(unsigned short index) const;
    /// removes and destroys all vertex tracks making up this animation
    void DestroyAllVertexTracks();
    /// applies all vertex tracks given a specific time point and weight to a given entity
    /**
        @remarks
        @param entity The Entity to which this animation should be applied
        @param timePos The time position in the animation to apply.
    */
    void Apply(Entity* entity, float timePos);

private:
    core::String name;
    float length;
    mutable ce_dynamic_array vertexTracks;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
