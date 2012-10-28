//------------------------------------------------------------------------------
//  animationstate.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "animationstate.h"
#include "chrisslymath.h"

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
AnimationState::AnimationState(const char* animName, float length, bool enabled) :
    animationName(animName),
    timePos(0.0f),
    enabled(enabled),
    loop(false),
    length(length)
{

}

//------------------------------------------------------------------------------
/**
*/
AnimationState::~AnimationState()
{

}

//------------------------------------------------------------------------------
/**
*/
void
AnimationState::SetEnabled(bool enabled)
{
    this->enabled = enabled;
}
   
//------------------------------------------------------------------------------
/**
*/
void
AnimationState::SetLoop(bool loop)
{
    this->loop = loop;
}

//------------------------------------------------------------------------------
/**
*/
void
AnimationState::SetTimePosition(float timePos)
{
    this->timePos = timePos;

    if (this->loop)
    {
        this->timePos = core::Math::Fmod(this->timePos, this->length);
        if (this->timePos < 0.0f)
        {
            this->timePos += this->length;
        }
    }
    else
    {
        if (this->timePos < 0.0f)
        {
            this->timePos = 0.0f;
        }
        else if (this->timePos > this->length)
        {
            this->timePos = this->length;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
AnimationState::AddTime(float offset)
{
    this->SetTimePosition(this->timePos + offset);
}

//------------------------------------------------------------------------------
/**
*/
bool
AnimationState::GetEnabled() const
{
    return this->enabled;
}

//------------------------------------------------------------------------------
/**
*/   
bool
AnimationState::GetLoop() const
{
    return this->loop;
}

//------------------------------------------------------------------------------
/**
*/
const char*
AnimationState::GetAnimationName() const
{
    return this->animationName;
}

//------------------------------------------------------------------------------
/**
*/
float
AnimationState::GetTimePosition() const
{
    return this->timePos;
}

} // namespace graphics
} // namespace chrissly
