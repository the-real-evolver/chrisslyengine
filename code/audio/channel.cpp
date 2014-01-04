//------------------------------------------------------------------------------
//  channel.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "channel.h"
#include "audiosystem.h"
#include <stdio.h>

namespace chrissly
{
namespace audio
{

//------------------------------------------------------------------------------
/**
*/
Channel::Channel() :
   currentSound(NULL),
   index(CHANNEL_FREE),
   position(0),
   isPlaying(false)
{

}

//------------------------------------------------------------------------------
/**
*/
Channel::~Channel()
{

}

//------------------------------------------------------------------------------
/**
*/
bool
Channel::GetCurrentSound(Sound** sound)
{
    *sound = this->currentSound;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
Channel::GetIndex(int* index)
{
    *index = this->index;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
Channel::GetPosition(unsigned int* position)
{
    *position = this->position;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
Channel::IsPlaying(bool* isplaying)
{
    *isplaying = this->isPlaying;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
Channel::SetPosition(unsigned int position)
{
    this->position = position;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
Channel::Stop()
{
    if (this->index != CHANNEL_FREE)
    {
        AudioSystem::Instance()->_GetAudioRenderer()->ReleaseChannel(this);
        this->index = CHANNEL_FREE;
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
Channel::_AttachSound(Sound* sound)
{
    this->currentSound = sound;
}

//------------------------------------------------------------------------------
/**
*/
void
Channel::_SetIndex(int index)
{
    this->index = index;
}

//------------------------------------------------------------------------------
/**
*/
void
Channel::_SetIsPlaying(bool isplaying)
{
    this->isPlaying = isplaying;
}

} // namespace audio
} // namespace chrissly