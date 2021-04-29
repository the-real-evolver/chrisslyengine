//------------------------------------------------------------------------------
//  audiosystem.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "audiosystem.h"
#include "wavcodec.h"
#include "vorbiscodec.h"
#include "dsp/mixer.h"
#include "dsp/filters.h"
#include "memoryallocatorconfig.h"
#include "chrisslymath.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>

namespace chrissly
{
namespace audio
{

using namespace chrissly::core;

AudioSystem* AudioSystem::Singleton = NULL;

static float DistanceFilterStore[32U] = {0.0f};

//------------------------------------------------------------------------------
/**
*/
AudioSystem::AudioSystem()
{
    Singleton = this;
    this->activeRenderer = CE_NEW AudioRenderer();
}

//------------------------------------------------------------------------------
/**
*/
AudioSystem::~AudioSystem()
{
    this->Release();
    CE_DELETE this->activeRenderer;
    Singleton = NULL;
}

//------------------------------------------------------------------------------
/**
*/
Result
AudioSystem::Initialise(void* const customParams)
{
    this->activeRenderer->Initialise(customParams);

    ce_dynamic_array_init(&this->soundPool, 8U);
    unsigned int i;
    for (i = 0U; i < this->soundPool.capacity; ++i)
    {
        Sound* sound = CE_NEW Sound();
        ce_dynamic_array_set(&this->soundPool, i, sound);
    }

    ce_dynamic_array_init(&this->dspPool, 8U);
    for (i = 0U; i < this->dspPool.capacity; ++i)
    {
        DSP* dsp = CE_NEW DSP();
        ce_dynamic_array_set(&this->dspPool, i, dsp);
    }

    ce_dynamic_array_init(&this->channelPool, this->activeRenderer->GetNumHardwareChannels());
    for (i = 0U; i < this->channelPool.capacity; ++i)
    {
        Channel* channel = CE_NEW Channel();
        ce_dynamic_array_set(&this->channelPool, i, channel);
    }

    this->activeRenderer->StartAudioProcessing();

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
AudioSystem::Release()
{
    this->activeRenderer->StopAudioProcessing();

    unsigned int i;
    for (i = 0U; i < this->channelPool.capacity; ++i)
    {
        Channel* channel = (Channel*)ce_dynamic_array_get(&this->channelPool, i);
        int index;
        channel->GetIndex(&index);
        if (index != Channel::CHANNEL_FREE)
        {
            this->activeRenderer->ReleaseChannel(channel);
        }
        CE_DELETE channel;
    }
    ce_dynamic_array_delete(&this->channelPool);

    for (i = 0U; i < this->dspPool.capacity; ++i)
    {
        CE_DELETE(DSP*)ce_dynamic_array_get(&this->dspPool, i);
    }
    ce_dynamic_array_delete(&this->dspPool);

    for (i = 0U; i < this->soundPool.capacity; ++i)
    {
        CE_DELETE (Sound*)ce_dynamic_array_get(&this->soundPool, i);
    }
    ce_dynamic_array_delete(&this->soundPool);

    this->activeRenderer->Shutdown();

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
AudioSystem::CreateSound(const char* const name, Mode mode, Sound** sound)
{
    *sound = NULL;
    unsigned int i;
    for (i = 0U; i < this->soundPool.capacity; ++i)
    {
        Sound *snd = (Sound*)ce_dynamic_array_get(&this->soundPool, i);
        if (!snd->IsInUse())
        {
            char* ext = (char*)strrchr(name, '.');
            if (NULL == ext)
            {
                return ERR_PLUGIN_MISSING;
            }
            ++ext;
            Codec* codec;
            if (0 == strncmp(ext, "wav", 3U))
            {
                codec = CE_NEW WavCodec();
            }
            else if (0 == strncmp(ext, "ogg", 3U))
            {
                codec = CE_NEW VorbisCodec();
            }
            else
            {
                return ERR_PLUGIN_MISSING;
            }

            snd->Setup(name, mode, codec);

            *sound = snd;

            return OK;
        }
    }

    return ERR_MEMORY;
}

//------------------------------------------------------------------------------
/**
*/
Result
AudioSystem::CreateDSP(const DspDescription* const desc, DSP** const dsp)
{
    unsigned int i;
    for (i = 0U; i < this->dspPool.capacity; ++i)
    {
        DSP* newDsp = (DSP*)ce_dynamic_array_get(&this->dspPool, i);
        if (!newDsp->IsInUse())
        {
            newDsp->Setup(desc);
            *dsp = newDsp;
            return OK;
        }
    }

    *dsp = NULL;

    return ERR_MEMORY;
}

//------------------------------------------------------------------------------
/**
*/
Result
AudioSystem::PlaySound(int channelid, Sound* const sound, bool paused, Channel** channel)
{
    if (NULL == sound)
    {
        return ERR_INVALID_PARAM;
    }

    if (sound->mode & MODE_CREATESTREAM)
    {
        CE_ASSERT(0U == sound->useCount, "AudioSystem::PlaySound(): multiple channels cannot share a streaming sound\n");
    }

    unsigned int i;
    for (i = 0U; i < this->channelPool.capacity; ++i)
    {
        Channel* chn = (Channel*)ce_dynamic_array_get(&this->channelPool, i);
        int index;
        chn->GetIndex(&index);
        if (Channel::CHANNEL_FREE == index)
        {
            if (channelid != Channel::CHANNEL_FREE)
            {
                CE_ASSERT(channelid < (int)this->channelPool.capacity, "AudioSystem::PlaySound(): invalid channelid '%i' (exceeds channel range '0 - %i')\n", channelid, this->channelPool.capacity - 1U);
                chn->SetIndex(channelid);
            }

            audio::Mode mode;
            sound->GetMode(&mode);
            if (mode & MODE_3D)
            {
                DspDescription dspDesc = {DistanceFilterCallback, chn};
                DSP* dsp;
                CreateDSP(&dspDesc, &dsp);
                chn->SetUserData((void*)i);
                chn->AddDSP(0U, dsp);
                memset(DistanceFilterStore + 16U * i, 0, 16U);
            }

            chn->AttachSound(sound);
            chn->SetPaused(paused);
            chn->SetPosition(0U);
            chn->SetVolume(1.0f);
            chn->SetPan(0.0f);
            chn->SetAttenuationFactor(0.0f);

            this->activeRenderer->StartChannel(chn);

            *channel = chn;

            return OK;
        }
    }

    *channel = NULL;

    return ERR_CHANNEL_ALLOC;
}

//------------------------------------------------------------------------------
/**
*/
Result
AudioSystem::Update()
{
    Vector3 sideVec = this->listenerUp.CrossProduct(this->listenerForward);
    sideVec.Normalise();

    unsigned int i;
    for (i = 0U; i < this->channelPool.capacity; ++i)
    {
        Channel* channel = (Channel*)ce_dynamic_array_get(&this->channelPool, i);
        int index;
        channel->GetIndex(&index);
        if (index != Channel::CHANNEL_FREE)
        {
            audio::Mode mode;
            channel->GetMode(&mode);
            if (mode & audio::MODE_3D)
            {
                float minDistance, maxDistance;
                channel->Get3DMinMaxDistance(&minDistance, &maxDistance);

                Vector3 channelPos;
                channel->Get3DAttributes(&channelPos);
                Vector3 distanceVec = channelPos - this->listenerPos;
                float distance = distanceVec.Length();
                if (distance >= minDistance && distance <= maxDistance)
                {
                    // note that sideVec points to the "left" and the order of parameters of the atan2 function is (y, x) not (x, y)
                    channel->SetPan(-Math::Cos(Math::ATan2(distanceVec.DotProduct(this->listenerForward), distanceVec.DotProduct(sideVec))));
                    channel->SetAttenuationFactor(1.0f - (distance - minDistance) / (maxDistance - minDistance));
                }
                else
                {
                    channel->SetAttenuationFactor(0.0f);
                }
            }

            this->activeRenderer->UpdateChannel(channel);
        }
    }

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
AudioSystem::Set3DListenerAttributes(const Vector3* const pos, const Vector3* const forward, const Vector3* const up)
{
    if (pos != NULL)
    {
        this->listenerPos = *pos;
    }
    if (forward != NULL)
    {
        this->listenerForward = *forward;
    }
    if (up != NULL)
    {
        this->listenerUp = *up;
    }

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
void
AudioSystem::_Mix(unsigned int numSamples, unsigned char* const buffer)
{
    memset(buffer, 0, numSamples << 2U);
    unsigned int i;
    for (i = 0U; i < this->channelPool.capacity; ++i)
    {
        Channel* channel = (Channel*)ce_dynamic_array_get(&this->channelPool, i);
        int index;
        channel->GetIndex(&index);
        if (index != Channel::CHANNEL_FREE)
        {
            bool isPlaying;
            channel->IsPlaying(&isPlaying);
            if (isPlaying)
            {
                bool paused;
                channel->GetPaused(&paused);
                if (!paused)
                {
                    Mode mode;
                    channel->GetMode(&mode);
                    float volume, pan;
                    channel->GetVolume(&volume);
                    channel->GetPan(&pan);
                    unsigned int position, length;
                    channel->GetPosition(&position);
                    Sound* sound;
                    channel->GetCurrentSound(&sound);
                    int numChannels, bits;
                    sound->GetFormat(NULL, NULL, &numChannels, &bits);
                    sound->GetLength(&length);
                    void* sampleBuffer = NULL;
                    if (mode & audio::MODE_3D)
                    {
                        volume *= channel->GetAttenuationFactor();
                    }
                    if (position + numSamples >= length)
                    {
                        sampleBuffer = channel->FillOutputBuffer(length - position, position);
                        ce_audio_mix_s16_stereo(bits, numChannels, sampleBuffer, (short*)buffer, length - position, volume, pan);
                        if (mode & MODE_LOOP_NORMAL)
                        {
                            if (length >= numSamples)
                            {
                                sampleBuffer = channel->FillOutputBuffer(position + numSamples - length, 0U);
                                ce_audio_mix_s16_stereo(bits, numChannels, sampleBuffer, (short*)buffer + (uintptr_t)((length - position) << 1U), position + numSamples - length, volume, pan);
                                channel->SetPosition(position + numSamples - length);
                            }
                        }
                        else
                        {
                            channel->ReleaseInternal();
                        }
                    }
                    else
                    {
                        sampleBuffer = channel->FillOutputBuffer(numSamples, position);
                        ce_audio_mix_s16_stereo(bits, numChannels, sampleBuffer, (short*)buffer, numSamples, volume, pan);
                        channel->SetPosition(position + numSamples);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
AudioRenderer* const
AudioSystem::GetAudioRenderer() const
{
    return this->activeRenderer;
}

//------------------------------------------------------------------------------
/**
*/
Result
AudioSystem::DistanceFilterCallback(int numChannels, int bits, unsigned int numSamples, const void* const inBuffer, void* const outBuffer, void* const dspUserData)
{
    Channel* channel = (Channel*)dspUserData;
    void* userData;
    channel->GetUserData(&userData);
    ce_audio_filter_lowpass(bits, numChannels, inBuffer, outBuffer, numSamples, channel->GetAttenuationFactor(), DistanceFilterStore + 4U * (unsigned int)userData);
    return OK;
}

} // namespace audio
} // namespace chrissly
