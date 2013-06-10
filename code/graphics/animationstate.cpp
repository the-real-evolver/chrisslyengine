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

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
AnimationState::AnimationState(const char* animName, float length, bool enabled) :
    timePos(0.0f),
    enabled(enabled),
    loop(false),
    length(length)
{
	this->animationName = animName;
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
        this->timePos = Math::Fmod(this->timePos, this->length);
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
    return this->animationName.C_Str();
}

//------------------------------------------------------------------------------
/**
*/
float
AnimationState::GetTimePosition() const
{
    return this->timePos;
}

//------------------------------------------------------------------------------
/**
*/
void
AnimationState::SetLength(float len)
{
    this->length = len;
}

} // namespace graphics
} // namespace chrissly
