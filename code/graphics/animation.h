#ifndef ANIMATION_H_
#define ANIMATION_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::Animation

    (C) 2012 Christian Bleicher
*/
#include "animationtrack.h"
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
    Animation(const char* const name, float length);
    /// destructor
    ~Animation();

    /// gets the name of this animation
    const char* const GetName() const;
    /// gets the total length of the animation
    float GetLength() const;
    /// creates a VertexAnimationTrack for animating vertex position data
    VertexAnimationTrack* const CreateVertexTrack(unsigned char handle);
    /// gets the number of VertexAnimationTrack objects contained in this animation
    unsigned short GetNumVertexTracks() const;
    /// gets a Vertex track by it's index
    VertexAnimationTrack* const GetVertexTrack(unsigned short index) const;
    /// removes and destroys all vertex tracks making up this animation
    void DestroyAllVertexTracks();

    /// creates a BoneAnimationTrack for animating the skeleton
    BoneAnimationTrack* const CreateBoneTrack(unsigned int numKeyFrames);
    /// gets the number of BoneAnimationTrack objects contained in this animation
    unsigned short GetNumBoneTracks() const;
    /// gets a Bone track by it's index
    BoneAnimationTrack* const GetBoneTrack(unsigned short index) const;
    /// removes and destroys all bone tracks making up this animation
    void DestroyAllBoneTracks();

    /// applies all tracks given a specific time point to a given entity
    /**
        @remarks
        @param entity The Entity to which this animation should be applied
        @param timePos The time position in the animation to apply
    */
    void Apply(Entity* const entity, float timePos);

private:
    core::String name;
    float length;
    VertexAnimationTrack** vertexTracks;
    BoneAnimationTrack** boneTracks;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
