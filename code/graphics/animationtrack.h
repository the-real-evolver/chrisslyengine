#ifndef ANIMATIONTRACK_H_
#define ANIMATIONTRACK_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::VertexAnimationTrack

    (C) 2012 Christian Bleicher
*/
#include "keyframe.h"
#include "dynamicarray.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

/**
    type of vertex animation
@par
    Morph animation is a simple approach where we have a whole series of
    snapshots of vertex data which must be interpolated, e.g. a running
    animation implemented as morph targets. Because this is based on simple
    snapshots, it's quite fast to use when animating an entire mesh because
    it's a simple linear change between keyframes. However, this simplistic
    approach does not support blending between multiple morph animations.
*/
enum VertexAnimationType
{
    /// no animation
    VAT_NONE = 0,
    /// morph animation is made up of many interpolated snapshot keyframes
    VAT_MORPH = 1
};

class VertexAnimationTrack
{
public:
    /// constructor with submesh handle
    VertexAnimationTrack(unsigned char handle);
    /// destructor
    ~VertexAnimationTrack();

    /// get the handle associated with this track
    unsigned char GetHandle() const;
    /// creates a new morph KeyFrame and adds it to this animation at the given time index
    /**
        @param timePos the time from which this KeyFrame will apply
    */
    VertexMorphKeyFrame* CreateVertexMorphKeyFrame(float timePos);
    /// returns the number of keyframes in this animation
    unsigned short GetNumKeyFrames() const;
    /// returns the morph KeyFrame at the specified index
    VertexMorphKeyFrame* GetVertexMorphKeyFrame(unsigned short index) const;
    /// removes all the KeyFrames from this track
    void RemoveAllKeyFrames();
    /// applies an animation track to the designated target
    /**
        @param timeIndex the time position in the animation to apply
    */
    void ApplyToVertexData(VertexData* data, int timeIndex);

private:
    unsigned char handle;
    mutable DynamicArray keyFrames;
    unsigned short numKeyFrames;
    int currentTimeIndex;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
