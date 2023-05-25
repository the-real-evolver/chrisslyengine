//------------------------------------------------------------------------------
//  channelbase.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "channelbase.h"
#include "audiosystem.h"
#include "chrisslyarray.h"
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
    samplePosition(0U),
    minDistance(1.0f),
    maxDistance(10000.0f),
    currentSound(NULL),
    index(CHANNEL_FREE),
    attenuationFactor(0.0f),
    propertiesHaveChanged(UNCHANGED),
    dsps(NULL),
    userData(NULL)
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
        AudioSystem::Instance()->GetAudioRenderer()->ReleaseChannel((Channel*)this);
    }

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::IsPlaying(bool* const isplaying)
{
    *isplaying = this->isPlaying;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::SetPaused(bool pause)
{
    this->paused = pause;

    this->propertiesHaveChanged |= PROPERTY_PAUSED;

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::GetPaused(bool* const pause)
{
    *pause = this->paused;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::SetVolume(float vol)
{
    if (vol > 1.0f)
    {
        this->volume = 1.0f;
    }
    else if (vol < 0.0f)
    {
        this->volume = 0.0f;
    }
    else
    {
        this->volume = vol;
    }

    this->propertiesHaveChanged |= PROPERTY_VOLUME;

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::GetVolume(float* const vol)
{
    *vol = this->volume;
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
        this->volume = Math::Pow(10.0f, decibel * 0.05f);
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
ChannelBase::GetPan(float* const pan)
{
    *pan = this->panning;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::SetMode(Mode modeflags)
{
    if (modeflags & (MODE_LOOP_OFF | MODE_LOOP_NORMAL))
    {
        this->mode &= ~(MODE_LOOP_OFF | MODE_LOOP_NORMAL);

        if (modeflags & MODE_LOOP_OFF)
        {
            this->mode |= MODE_LOOP_OFF;
        }
        else if (modeflags & MODE_LOOP_NORMAL)
        {
            this->mode |= MODE_LOOP_NORMAL;
        }
    }

    if (modeflags & MODE_3D)
    {
        this->mode &= ~MODE_2D;
        this->mode |= MODE_3D;
    }
    else if (modeflags & MODE_2D)
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
ChannelBase::GetMode(Mode* const modeflags)
{
    *modeflags = this->mode;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::SetPosition(unsigned int position)
{
    this->samplePosition = position;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::GetPosition(unsigned int* const position)
{
    *position = this->samplePosition;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::Set3DAttributes(const Vector3* const position)
{
    if (position != NULL)
    {
        this->spatialPosition = *position;
    }
    return OK;
}

 //------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::Get3DAttributes(Vector3* const position)
{
    if (position != NULL)
    {
        *position = this->spatialPosition;
    }
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::Set3DMinMaxDistance(float mindistance, float maxdistance)
{
    CE_ASSERT(mindistance >= 0.0f && maxdistance > mindistance, "Channel::Set3DMinMaxDistance(): invalid parameters\n");

    this->minDistance = mindistance;
    this->maxDistance = maxdistance;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::Get3DMinMaxDistance(float* const mindistance, float* const maxdistance)
{
    if (mindistance != NULL)
    {
        *mindistance = this->minDistance;
    }
    if (maxdistance != NULL)
    {
        *maxdistance = this->maxDistance;
    }
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::SetUserData(void* const data)
{
    this->userData = data;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::GetUserData(void** const data)
{
    *data = this->userData;
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
ChannelBase::GetIndex(int* const idx)
{
    *idx = this->index;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::AddDSP(unsigned int idx, DSP* const dsp)
{
    if (NULL == dsp) return ERR_INVALID_PARAM;

    if ((idx + 1U) > ce_array_size(this->dsps))
    {
        this->dsps = ce_array_grow_wrapper(this->dsps, sizeof(*(this->dsps)), idx + 1U);
        ce_array_header(this->dsps)->size = idx + 1U;
    }

    if (this->dsps[idx] != NULL && this->dsps[idx] != dsp) this->dsps[idx]->Release();

    this->dsps[idx] = dsp;

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
ChannelBase::RemoveDSP(DSP* const dsp)
{
    if (NULL == dsp) return ERR_INVALID_PARAM;

    unsigned int i;
    for (i = 0U; i < ce_array_size(this->dsps); ++i)
    {
        if (this->dsps[i] == dsp)
        {
            this->dsps[i] = NULL;
            dsp->Release();
        }
    }

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
void
ChannelBase::AttachSound(Sound* const sound)
{
    sound->GetMode(&this->mode);
    this->currentSound = sound;
}

//------------------------------------------------------------------------------
/**
*/
void
ChannelBase::SetIndex(int idx)
{
    this->index = idx;
}

//------------------------------------------------------------------------------
/**
*/
void
ChannelBase::SetIsPlaying(bool isplaying)
{
    this->isPlaying = isplaying;
}

//------------------------------------------------------------------------------
/**
*/
void
ChannelBase::SetAttenuationFactor(float attenuation)
{
    this->attenuationFactor = attenuation;

    this->propertiesHaveChanged |= PROPERTY_ATTENUATION;
}

//------------------------------------------------------------------------------
/**
*/
float
ChannelBase::GetAttenuationFactor() const
{
    return this->attenuationFactor;
}

//------------------------------------------------------------------------------
/**
*/
PropertyChange
ChannelBase::PropertiesHaveChanged()
{
    PropertyChange propsHaveChanged = this->propertiesHaveChanged;
    this->propertiesHaveChanged = UNCHANGED;
    return propsHaveChanged;
}

//------------------------------------------------------------------------------
/**
*/
const core::Mutex&
ChannelBase::GetSyncLock() const
{
    return this->syncLock;
}

//------------------------------------------------------------------------------
/**
*/
void* const
ChannelBase::FillOutputBuffer(unsigned int numSamples, unsigned int position)
{
    void* buffer = this->mode & MODE_CREATESTREAM ? this->currentSound->GetCodec()->FillStreamBuffer(numSamples, position) : this->currentSound->GetSampleBufferPointer(position);

    // apply dsps
    int numChannels, bits;
    this->currentSound->GetFormat(NULL, NULL, &numChannels, &bits);
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->dsps); ++i)
    {
        DSP* dsp = this->dsps[i];
        if (NULL == dsp) continue;
        bool bypass;
        dsp->GetBypass(&bypass);
        if (bypass) continue;
        dsp->process(dsp, numChannels, bits, numSamples, buffer, dsp->buffer);
        buffer = dsp->buffer;
    }

    return buffer;
}

//------------------------------------------------------------------------------
/**
*/
void
ChannelBase::ReleaseInternal()
{
    // release and detach dsps
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->dsps); ++i)
    {
        if (this->dsps[i] != NULL) this->dsps[i]->Release();
    }
    ce_array_delete(this->dsps);
    this->isPlaying = false;
    this->index = CHANNEL_FREE;
    this->currentSound->DecrementUseCount();
}

} // namespace audio
} // namespace chrissly
