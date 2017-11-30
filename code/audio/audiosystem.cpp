//------------------------------------------------------------------------------
//  audiosystem.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "audiosystem.h"
#include "wavcodec.h"
#include "vorbiscodec.h"
#include "dsp/mixer.h"
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
    Sound* snd = NULL;
    unsigned int i;
    for (i = 0U; i < this->soundPool.capacity; ++i)
    {
        snd = (Sound*)ce_dynamic_array_get(&this->soundPool, i);
        if (!snd->IsInUse())
        {
            break;
        }
    }
    if (i == this->soundPool.capacity)
    {
        return ERR_MEMORY;
    }

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

//------------------------------------------------------------------------------
/**
*/
Result
AudioSystem::PlaySound(int channelid, Sound* const sound, bool paused, Channel** channel)
{
    *channel = NULL;
    Channel* chn = NULL;
    unsigned int i;
    for (i = 0U; i < this->channelPool.capacity; ++i)
    {
        chn = (Channel*)ce_dynamic_array_get(&this->channelPool, i);
        int index;
        chn->GetIndex(&index);
        if (Channel::CHANNEL_FREE == index)
        {
            break;
        }
    }
    if (i == this->channelPool.capacity)
    {
        return ERR_CHANNEL_ALLOC;
    }

    if (channelid != Channel::CHANNEL_FREE)
    {
        CE_ASSERT(channelid < (int)this->channelPool.capacity, "AudioSystem::PlaySound(): invalid channelid '%i' (exceeds channel range '0 - %i')\n", channelid, this->channelPool.capacity - 1U);
        chn->_SetIndex(channelid);
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
                    channel->SetPan(-Math::Sin(Math::ATan2(distanceVec.DotProduct(sideVec), distanceVec.DotProduct(this->listenerForward))));
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
                        volume *= channel->_GetAttenuationFactor();
                    }
                    if (position + numSamples >= length)
                    {
                        sampleBuffer = mode & MODE_CREATESTREAM ? sound->_GetCodec()->FillStreamBuffer(length - position, position) : sound->_GetSampleBufferPointer(position);
                        ce_audio_mix_s16_stereo(bits, numChannels, sampleBuffer, (short*)buffer, length - position, volume, pan);
                        if (mode & MODE_LOOP_NORMAL)
                        {
                            if (length >= numSamples)
                            {
                                sampleBuffer = mode & MODE_CREATESTREAM ? sound->_GetCodec()->FillStreamBuffer(position + numSamples - length, 0U) : sound->_GetSampleBufferPointer(0U);
                                ce_audio_mix_s16_stereo(bits, numChannels, sampleBuffer, (short*)buffer + (uintptr_t)((length - position) << 1U), position + numSamples - length, volume, pan);
                                channel->SetPosition(position + numSamples - length);
                            }
                        }
                        else
                        {
                            channel->_SetIsPlaying(false);
                            channel->_SetIndex(Channel::CHANNEL_FREE);
                            sound->_DecrementUseCount();
                        }
                    }
                    else
                    {
                        sampleBuffer = mode & MODE_CREATESTREAM ? sound->_GetCodec()->FillStreamBuffer(numSamples, position) : sound->_GetSampleBufferPointer(position);
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

} // namespace audio
} // namespace chrissly
