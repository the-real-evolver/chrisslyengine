//------------------------------------------------------------------------------
//  audiosystem.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "audiosystem.h"
#include "wavcodec.h"
#include "memoryallocatorconfig.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>

namespace chrissly
{
namespace audio
{

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
bool
AudioSystem::Initialise(void* customParams)
{
    DynamicArrayInit(&this->staticSounds, 0);
    this->numStaticSounds = 0;

    DynamicArrayInit(&this->channelPool, 8);
    unsigned int i;
    for (i = 0; i < this->channelPool.cur_size; i++)
    {
        Channel* channel = CE_NEW Channel();
        if (!DynamicArraySet(&this->channelPool, i, channel))
        {
            CE_ASSERT(false, "AudioSystem::Initialise(): DynamicArraySet failed\n");
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
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
        CE_DELETE (Channel*)DynamicArrayGet(&this->channelPool, i);
    }
    DynamicArrayDelete(&this->channelPool);

    for (i = 0; i < this->numStaticSounds; i++)
    {
        CE_DELETE (Sound*)DynamicArrayGet(&this->staticSounds, i);
    }
    DynamicArrayDelete(&this->staticSounds);
    this->numStaticSounds = 0;

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
AudioSystem::CreateSound(const char* name, Mode mode, Sound** sound)
{
    char* ext = strrchr(name, '.');
    if (NULL == ext) return false;
    ext++;
    Codec* codec;
    if (0 == strcmp(ext, "wav"))
    {
        codec = CE_NEW WavCodec();
    }
    else
    {
        return false;
    }

    Sound* snd = CE_NEW Sound();
    snd->_Setup(name, mode, codec);
    *sound = snd;

    if (MODE_DEFAULT == mode || mode & MODE_CREATESAMPLE)
    {
        bool result = DynamicArraySet(&this->staticSounds, this->numStaticSounds, snd);
        CE_ASSERT(result, "AudioSystem::CreateSound(): can't add sound to staticSounds");
        this->numStaticSounds++;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
AudioSystem::PlaySound(int channelid, Sound* sound, Channel** channel)
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
    if (i == this->channelPool.cur_size) return false;

    if (channelid != Channel::CHANNEL_FREE)
    {
        CE_ASSERT(channelid < (int)this->channelPool.cur_size, "AudioSystem::PlaySound(): invalid channelid '%i', valid range [0 - %i]", channelid, this->channelPool.cur_size);
        chn->_SetIndex(channelid);
    }

    chn->_AttachSound(sound);

    this->activeRenderer->StartChannel(chn);

    *channel = chn;

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
AudioSystem::Update()
{
    unsigned int i;
    for (i = 0; i < this->channelPool.cur_size; i++)
    {
        Channel* channel = (Channel*)DynamicArrayGet(&this->channelPool, i);
        int index;
        channel->GetIndex(&index);
        if (index != Channel::CHANNEL_FREE)
        {
            this->activeRenderer->UpdateChannel(channel);
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
AudioRenderer*
AudioSystem::_GetAudioRenderer()
{
    return this->activeRenderer;
}

} // namespace audio
} // namespace chrissly