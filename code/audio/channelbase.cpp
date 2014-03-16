//------------------------------------------------------------------------------
//  channelbase.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "channelbase.h"
#include "audiosystem.h"
#include <stdio.h>

namespace chrissly
{
namespace audio
{

//------------------------------------------------------------------------------
/**
*/
ChannelBase::ChannelBase() :
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
ChannelBase::~ChannelBase()
{

}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::GetCurrentSound(Sound** sound)
{
    *sound = this->currentSound;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::GetIndex(int* index)
{
    *index = this->index;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::GetPosition(unsigned int* position)
{
    *position = this->position;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::IsPlaying(bool* isplaying)
{
    *isplaying = this->isPlaying;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::SetPosition(unsigned int position)
{
    this->position = position;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::Stop()
{
    if (this->index != CHANNEL_FREE)
    {
        AudioSystem::Instance()->_GetAudioRenderer()->ReleaseChannel((Channel*)this);
        this->index = CHANNEL_FREE;
    }

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::SetVolume(float volume)
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
ChannelBase::GetVolume(float* volume)
{
    *volume = this->volume;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::SetPan(float pan)
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
ChannelBase::GetPan(float* pan)
{
    *pan = this->panning;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::SetMode(Mode mode)
{
    this->mode = mode;

    this->propertiesHasChanged = true;

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::GetMode(Mode* mode)
{
    *mode = this->mode;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
void
ChannelBase::_AttachSound(Sound* sound)
{
    sound->GetMode(&this->mode);
    this->currentSound = sound;
}

//------------------------------------------------------------------------------
/**
*/
void
ChannelBase::_SetIndex(int index)
{
    this->index = index;
}

//------------------------------------------------------------------------------
/**
*/
void
ChannelBase::_SetIsPlaying(bool isplaying)
{
    this->isPlaying = isplaying;
}

//------------------------------------------------------------------------------
/**
*/
bool
ChannelBase::_PropertiesHasChanged()
{
    bool propertiesHasChanged = this->propertiesHasChanged;
    this->propertiesHasChanged = false;
    return propertiesHasChanged;
}

} // namespace audio
} // namespace chrissly