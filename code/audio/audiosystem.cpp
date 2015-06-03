//------------------------------------------------------------------------------
//  audiosystem.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "audiosystem.h"
#include "wavcodec.h"
#include "vorbiscodec.h"
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
    Singleton = NULL;
    this->Release();
    CE_DELETE this->activeRenderer;
}

//------------------------------------------------------------------------------
/**
*/
Result
AudioSystem::Initialise(void* customParams)
{
    this->activeRenderer->_Initialise(customParams);

    DynamicArrayInit(&this->soundPool, 8);
    unsigned int i;
    for (i = 0; i < this->soundPool.cur_size; i++)
    {
        Sound* sound = CE_NEW Sound();
        DynamicArraySet(&this->soundPool, i, sound);
    }

    DynamicArrayInit(&this->channelPool, this->activeRenderer->GetNumHardwareChannels());
    for (i = 0; i < this->channelPool.cur_size; i++)
    {
        Channel* channel = CE_NEW Channel();
        DynamicArraySet(&this->channelPool, i, channel);
    }

    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
AudioSystem::Release()
{
    unsigned int i;
    for (i = 0; i < this->channelPool.cur_size; i++)
    {
        Channel* channel = (Channel*)DynamicArrayGet(&this->channelPool, i);
        int index;
        channel->GetIndex(&index);
        if (index != Channel::CHANNEL_FREE)
        {
            this->activeRenderer->ReleaseChannel(channel);
        }
        CE_DELETE channel;
    }
    DynamicArrayDelete(&this->channelPool);

    for (i = 0; i < this->soundPool.cur_size; i++)
    {
        CE_DELETE (Sound*)DynamicArrayGet(&this->soundPool, i);
    }
    DynamicArrayDelete(&this->soundPool);

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
    for (i = 0; i < this->soundPool.cur_size; i++)
    {
        snd = (Sound*)DynamicArrayGet(&this->soundPool, i);
        if (!snd->_IsRealized()) break;
    }
    if (i == this->soundPool.cur_size) return ERR_MEMORY;

    char* ext = strrchr(name, '.');
    if (NULL == ext) return ERR_PLUGIN_MISSING;
    ext++;
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
    for (i = 0; i < this->channelPool.cur_size; i++)
    {
        chn = (Channel*)DynamicArrayGet(&this->channelPool, i);
        int index;
        chn->GetIndex(&index);
        if (Channel::CHANNEL_FREE == index) break;
    }
    if (i == this->channelPool.cur_size) return ERR_CHANNEL_ALLOC;

    if (channelid != Channel::CHANNEL_FREE)
    {
        CE_ASSERT(channelid < (int)this->channelPool.cur_size, "AudioSystem::PlaySound(): invalid channelid '%i' (exceeds channel range '0 - %i')\n", channelid, this->channelPool.cur_size - 1);
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
    for (i = 0; i < this->channelPool.cur_size; i++)
    {
        Channel* channel = (Channel*)DynamicArrayGet(&this->channelPool, i);
        int index;
        channel->GetIndex(&index);
        if (index != Channel::CHANNEL_FREE)
        {
            audio::Mode mode;
            channel->GetMode(&mode);
            if (mode & audio::MODE_CREATESTREAM)
            {
                audio::Sound* sound;
                channel->GetCurrentSound(&sound);
                sound->_GetCodec()->FillStreamBackBuffer();
            }

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
    if (pos != NULL) this->listenerPos = *pos;
    if (forward != NULL) this->listenerForward = *forward;
    if (up != NULL) this->listenerUp = *up;

    return OK;
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
