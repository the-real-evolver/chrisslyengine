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
AudioSystem::Initialise(void* customParams)
{
    this->activeRenderer->_Initialise(customParams);

    ce_dynamic_array_init(&this->soundPool, 8);
    unsigned int i;
    for (i = 0; i < this->soundPool.capacity; ++i)
    {
        Sound* sound = CE_NEW Sound();
        ce_dynamic_array_set(&this->soundPool, i, sound);
    }

    ce_dynamic_array_init(&this->channelPool, this->activeRenderer->GetNumHardwareChannels());
    for (i = 0; i < this->channelPool.capacity; ++i)
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
    for (i = 0; i < this->channelPool.capacity; ++i)
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

    for (i = 0; i < this->soundPool.capacity; ++i)
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
AudioSystem::CreateSound(const char* name, Mode mode, Sound** sound)
{
    Sound* snd = NULL;
    unsigned int i;
    for (i = 0; i < this->soundPool.capacity; ++i)
    {
        snd = (Sound*)ce_dynamic_array_get(&this->soundPool, i);
        if (!snd->_IsRealized())
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
    if (0 == strcmp(ext, "wav"))
    {
        codec = CE_NEW WavCodec();
    }
    else if (0 == strcmp(ext, "ogg"))
    {
        codec = CE_NEW VorbisCodec();
    }
    else
    {
        return ERR_PLUGIN_MISSING;
    }

    snd->_Setup(name, mode, codec);

    *sound = snd;

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
AudioSystem::PlaySound(int channelid, Sound* sound, bool paused, Channel** channel)
{
    Channel* chn = NULL;
    unsigned int i;
    for (i = 0; i < this->channelPool.capacity; ++i)
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
        CE_ASSERT(channelid < (int)this->channelPool.capacity, "AudioSystem::PlaySound(): invalid channelid '%i' (exceeds channel range '0 - %i')\n", channelid, this->channelPool.capacity - 1);
        chn->_SetIndex(channelid);
    }

    chn->_AttachSound(sound);
    chn->SetPaused(paused);
    chn->SetPosition(0);
    chn->SetVolume(1.0f);
    chn->SetPan(0.0f);
    chn->_SetAttenuationFactor(0.0f);

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
    for (i = 0; i < this->channelPool.capacity; ++i)
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
                    channel->_SetAttenuationFactor(1.0f - (distance - minDistance) / (maxDistance - minDistance));
                }
                else
                {
                    channel->_SetAttenuationFactor(0.0f);
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
AudioSystem::Set3DListenerAttributes(const Vector3* pos, const Vector3* forward, const Vector3* up)
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
AudioSystem::Mix(unsigned int numSamples, unsigned char* buffer)
{
    memset(buffer, 0, numSamples << 2U);
    unsigned int i;
    for (i = 0; i < this->channelPool.capacity; ++i)
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
                    if (position + numSamples >= length)
                    {
                        ce_audio_mix_s16_stereo(bits, numChannels, sound->_GetSampleBufferPointer(position), (short*)buffer, length - position, volume, pan);
                        if (mode & MODE_LOOP_NORMAL)
                        {
                            if (length >= numSamples)
                            {
                                ce_audio_mix_s16_stereo(bits, numChannels, sound->_GetSampleBufferPointer(0), (short*)buffer + (uintptr_t)(length - position), position + numSamples - length, volume, pan);
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
                        ce_audio_mix_s16_stereo(bits, numChannels, sound->_GetSampleBufferPointer(position), (short*)buffer, numSamples, volume, pan);
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
AudioRenderer*
AudioSystem::_GetAudioRenderer() const
{
    return this->activeRenderer;
}

} // namespace audio
} // namespace chrissly
