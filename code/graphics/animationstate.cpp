//------------------------------------------------------------------------------
//  animationstate.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "animationstate.h"
#include "chrisslymath.h"
#include "chrisslyarray.h"
#include "debug.h"

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
AnimationState::AnimationState(const char* const animName, float length, bool enabled) :
    timePos(0.0f),
    enabled(enabled),
    loop(false),
    length(length),
    weight(1.0f),
    blendMask(NULL)
{
    this->animationName = animName;
}

//------------------------------------------------------------------------------
/**
*/
AnimationState::~AnimationState()
{
    this->DestroyBlendMask();
}

//------------------------------------------------------------------------------
/**
*/
void
AnimationState::SetEnabled(bool enable)
{
    this->enabled = enable;
}

//------------------------------------------------------------------------------
/**
*/
void
AnimationState::SetLoop(bool looped)
{
    this->loop = looped;
}

//------------------------------------------------------------------------------
/**
*/
void
AnimationState::SetTimePosition(float timePosition)
{
    this->timePos = timePosition;

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

//------------------------------------------------------------------------------
/**
*/
float
AnimationState::GetLength() const
{
    return this->length;
}

//------------------------------------------------------------------------------
/**
*/
void
AnimationState::SetWeight(float w)
{
    this->weight = w;
}

//------------------------------------------------------------------------------
/**
*/
float
AnimationState::GetWeight() const
{
    return this->weight;
}

//------------------------------------------------------------------------------
/**
*/
void
AnimationState::CreateBlendMask(unsigned int sizeHint, float initialWeight)
{
    this->DestroyBlendMask();
    ce_array_init(this->blendMask, sizeHint);
    unsigned int i;
    for (i = 0; i < sizeHint; ++i)
    {
        ce_array_push_back(this->blendMask, initialWeight);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
AnimationState::DestroyBlendMask()
{
    if (this->blendMask != NULL)
    {
        ce_array_delete(this->blendMask);
        this->blendMask = NULL;
    }
}

//------------------------------------------------------------------------------
/**
*/
float*
AnimationState::GetBlendMask() const
{
    return this->blendMask;
}

//------------------------------------------------------------------------------
/**
*/
void
AnimationState::SetBlendMaskEntry(unsigned int index, float entryWeight)
{
    CE_ASSERT(index < ce_array_size(this->blendMask), "AnimationState::SetBlendMaskEntry(): invalid index");
    this->blendMask[index] = entryWeight;
}

} // namespace graphics
} // namespace chrissly
