//------------------------------------------------------------------------------
//  channelbase.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "channelbase.h"
#include "audiosystem.h"
#include "chrisslymath.h"
#include "debug.h"
#include <stdio.h>

namespace chrissly
{
namespace audio
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
ChannelBase::ChannelBase() :
    isPlaying(false),
    paused(false),
    volume(1.0f),
    panning(0.0f),
    mode(MODE_DEFAULT),
    position(0),
    minDistance(1.0f),
    maxDistance(10000.0f),
    currentSound(NULL),
    index(CHANNEL_FREE),
    attenuationFactor(0.0f),
    propertiesHaveChanged(UNCHANGED)
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
ChannelBase::Stop()
{
    if (this->index != CHANNEL_FREE)
    {
        AudioSystem::Instance()->_GetAudioRenderer()->ReleaseChannel((Channel*)this);
    }

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
ChannelBase::SetPaused(bool paused)
{
    this->paused = paused;

    this->propertiesHaveChanged |= PROPERTY_PAUSED;

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::GetPaused(bool* paused)
{
    *paused = this->paused;
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

    this->propertiesHaveChanged |= PROPERTY_VOLUME;

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
ChannelBase::SetPressureLevel(float decibel)
{
    if (decibel > 0.0f)
    {
        this->volume = 1.0f;
    }
    else if (decibel < -32.0f)
    {
        this->volume = 0.0f;
    }
    else
    {
        this->volume = Math::Pow(10, decibel / 20.0f);
    }

    this->propertiesHaveChanged |= PROPERTY_VOLUME;

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

    this->propertiesHaveChanged |= PROPERTY_PAN;

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
    if (mode & (MODE_LOOP_OFF | MODE_LOOP_NORMAL))
    {
        this->mode &= ~(MODE_LOOP_OFF | MODE_LOOP_NORMAL);

        if (mode & MODE_LOOP_OFF)
        {
            this->mode |= MODE_LOOP_OFF;
        }
        else if (mode & MODE_LOOP_NORMAL)
        {
            this->mode |= MODE_LOOP_NORMAL;
        }
    }

    if (mode & MODE_3D)
    {
        this->mode &= ~MODE_2D;
        this->mode |= MODE_3D;
    }
    else if (mode & MODE_2D)
    {
        this->mode &= ~MODE_3D;
        this->mode |= MODE_2D;
    }

    this->propertiesHaveChanged |= PROPERTY_MODE;

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
ChannelBase::GetPosition(unsigned int* position)
{
    *position = this->position;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::Set3DAttributes(const Vector3* pos)
{
    if (pos != NULL) this->pos = *pos;
    return OK;
}

 //------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::Get3DAttributes(Vector3* pos)
{
    if (pos != NULL) *pos = this->pos;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::Set3DMinMaxDistance(float mindistance, float maxdistance)
{
    CE_ASSERT(mindistance >= 0.0f && maxdistance > mindistance, "Channel::Set3DMinMaxDistance: invalid parameters\n");

    this->minDistance = mindistance;
    this->maxDistance = maxdistance;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::Get3DMinMaxDistance(float* mindistance, float* maxdistance)
{
    if (mindistance != NULL) *mindistance = this->minDistance;
    if (maxdistance != NULL) *maxdistance = this->maxDistance;
    return OK;
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
void
ChannelBase::_SetAttenuationFactor(float attenuation)
{
    this->attenuationFactor = attenuation;

    this->propertiesHaveChanged |= PROPERTY_ATTENUATION;
}

//------------------------------------------------------------------------------
/**
*/
float
ChannelBase::_GetAttenuationFactor() const
{
    return this->attenuationFactor;
}

//------------------------------------------------------------------------------
/**
*/
PropertyChange
ChannelBase::_PropertiesHaveChanged()
{
    PropertyChange propertiesHaveChanged = this->propertiesHaveChanged;
    this->propertiesHaveChanged = UNCHANGED;
    return propertiesHaveChanged;
}

//------------------------------------------------------------------------------
/**
*/
const core::Mutex&
ChannelBase::_GetSyncLock() const
{
    return this->syncLock;
}

} // namespace audio
} // namespace chrissly
