#ifndef ANIMATIONSTATE_H_
#define ANIMATIONSTATE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::AnimationState

    (C) 2012 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class AnimationState
{
public:
    /// constructor
    AnimationState(const char* animName, float length, bool enabled = false);
    /// destructor
    ~AnimationState();

    /// sets whether this animation is enabled
    void SetEnabled(bool enabled);
    /**
        sets whether or not an animation loops at the start and end of
        the animation if the time continues to be altereds
    */
    void SetLoop(bool loop);
    /// sets the time position for this animation
    void SetTimePosition(float timePos);
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
    const char* GetAnimationName() const;
    /// gets the time position for this animation
    float GetTimePosition() const;
    /// sets the total length of this animation (may be shorter than whole animation)
    void SetLength(float len);

private:
    const char* animationName;
    float timePos;
    bool enabled;
    bool loop;
    float length;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
