#ifndef ANIMATIONSTATE_H_
#define ANIMATIONSTATE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::AnimationState

    (C) 2012 Christian Bleicher
*/
#include "chrisslystring.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class AnimationState
{
public:
    /// constructor
    AnimationState(const char* const animName, float length, bool enabled = false);
    /// destructor
    ~AnimationState();

    /// sets whether this animation is enabled
    void SetEnabled(bool enable);
    /**
        sets whether or not an animation loops at the start and end of
        the animation if the time continues to be altered
    */
    void SetLoop(bool looped);
    /// sets the time position for this animation
    void SetTimePosition(float timePosition);
    /// Modifies the time position, adjusting for animation length
    /**
        @param
            offset the amount of time, in seconds, to extend the animation
        @remarks
            this method loops at the edges if animation looping is enabled
    */
    void AddTime(float offset);
    /// returns true if this animation is currently enabled
    bool GetEnabled() const;
    /// gets whether or not this animation loops
    bool GetLoop() const;
    /// gets the name of the animation to which this state applies
    const char* const GetAnimationName() const;
    /// gets the time position for this animation
    float GetTimePosition() const;
    /// sets the total length of this animation (may be shorter than whole animation)
    void SetLength(float len);
    /// gets the total length of this animation (may be shorter than whole animation)
    float GetLength() const;
    /// sets the weight of this animation
    void SetWeight(float w);
    /// gets the weight of this animation
    float GetWeight() const;
    /// create a new blend mask with the given number of entries
    /**
        In addition to assigning a single weight value to a skeletal animation,
        it may be desirable to assign animation weights per bone using a 'blend mask'.

        @param sizeHint
            the number of bones of the skeleton owning this AnimationState
        @param initialWeight
            the value all the blend mask entries will be initialised with
    */
    void CreateBlendMask(unsigned int sizeHint, float initialWeight = 1.0f);
    /// destroy the currently set blend mask
    void DestroyBlendMask();
    /// gets the current blend mask
    float* const GetBlendMask() const;
    /// sets the absolute (not modulated with the averaged weight) weight for the bone at the given index
    void SetBlendMaskEntry(unsigned int index, float entryWeight);

private:
    core::String animationName;
    float timePos;
    bool enabled;
    bool loop;
    float length;
    float weight;
    float* blendMask;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
