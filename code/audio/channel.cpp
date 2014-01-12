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
    isPlaying(false),
    volume(1.0f),
    panning(0.0f),
    mode(MODE_DEFAULT),
    propertiesHasChanged(false)
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
Result
Channel::GetCurrentSound(Sound** sound)
{
    *sound = this->currentSound;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
Channel::GetIndex(int* index)
{
    *index = this->index;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
Channel::GetPosition(unsigned int* position)
{
    *position = this->position;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
Channel::IsPlaying(bool* isplaying)
{
    *isplaying = this->isPlaying;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
Channel::SetPosition(unsigned int position)
{
    this->position = position;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
Channel::Stop()
{
    if (this->index != CHANNEL_FREE)
    {
        AudioSystem::Instance()->_GetAudioRenderer()->ReleaseChannel(this);
        this->index = CHANNEL_FREE;
    }

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
Channel::SetVolume(float volume)
{
    if (volume > 1.0f)
    {
        this->volume = 1.0f;
    }
    else if (volume < 0.0f)
    {
        this->volume = 0.0f;
    }
    else
    {
        this->volume = volume;
    }

    this->propertiesHasChanged = true;

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
Channel::GetVolume(float* volume)
{
    *volume = this->volume;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
Channel::SetPan(float pan)
{
    if (pan > 1.0f)
    {
        this->panning = 1.0f;
    }
    else if (pan < -1.0f)
    {
        this->panning = -1.0f;
    }
    else
    {
        this->panning = pan;
    }

    this->propertiesHasChanged = true;

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
Channel::GetPan(float* pan)
{
    *pan = this->panning;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
Channel::SetMode(Mode mode)
{
    this->mode = mode;

    this->propertiesHasChanged = true;

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
Channel::GetMode(Mode* mode)
{
    *mode = this->mode;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
void
Channel::_AttachSound(Sound* sound)
{
    sound->GetMode(&this->mode);
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

//------------------------------------------------------------------------------
/**
*/
bool
Channel::_PropertiesHasChanged()
{
    bool propertiesHasChanged = this->propertiesHasChanged;
    this->propertiesHasChanged = false;
    return propertiesHasChanged;
}

} // namespace audio
} // namespace chrissly