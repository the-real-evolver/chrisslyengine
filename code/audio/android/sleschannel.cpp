//------------------------------------------------------------------------------
//  sleschannel.cpp
//  (C) 2014 Christian Bleicher
//------------------------------------------------------------------------------
#include "sleschannel.h"
#include "slesaudiorenderer.h"
#include "debug.h"
#include <stdio.h>

namespace chrissly
{

//------------------------------------------------------------------------------
/**
*/
SLESChannel::SLESChannel() :
    bufferQueueInterface(NULL),
    playerInterface(NULL),
    volumeInterface(NULL),
    player(NULL),
    effectSendInterface(NULL)
{

}

//------------------------------------------------------------------------------
/**
*/
SLESChannel::~SLESChannel()
{

}

//------------------------------------------------------------------------------
/**
*/
void
SLESChannel::SetupAudioPlayer(SLEngineItf engineInterface, SLObjectItf outputMix)
{
    SLDataLocator_OutputMix locatorOutputmix;
    locatorOutputmix.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    locatorOutputmix.outputMix   = outputMix;
    SLDataSink audioSink;
    audioSink.pLocator = &locatorOutputmix;
    audioSink.pFormat  = NULL;

    SLDataSource audioSource = this->currentSound->GetAudioSource();
    const SLInterfaceID  ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_EFFECTSEND};
    const SLboolean required[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    SLresult result = (*engineInterface)->CreateAudioPlayer(engineInterface, &this->player, &audioSource, &audioSink, 3, ids, required);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to create audio player\n");
    result = (*player)->Realize(player, SL_BOOLEAN_FALSE);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to realize audio player\n");

    result = (*this->player)->GetInterface(this->player, SL_IID_PLAY, &this->playerInterface);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to get play interface\n");

    result = (*this->player)->GetInterface(this->player, SL_IID_BUFFERQUEUE, &this->bufferQueueInterface);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to get bufferqueue interface\n");

    result = (*this->bufferQueueInterface)->RegisterCallback(this->bufferQueueInterface, SLESAudioRenderer::BufferQueueCallback, this);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to register callback\n");

    result = (*this->player)->GetInterface(this->player, SL_IID_VOLUME, &this->volumeInterface);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to get volume interface\n");

    SLmillibel volume;
    result = (*this->volumeInterface)->GetMaxVolumeLevel(this->volumeInterface, &volume);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to get max volume level\n");

    result = (*this->volumeInterface)->SetVolumeLevel(this->volumeInterface, volume);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to set volume\n");

    result = (*this->player)->GetInterface(this->player, SL_IID_EFFECTSEND, &this->effectSendInterface);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to get effect send interface\n");
}

//------------------------------------------------------------------------------
/**
*/
void
SLESChannel::Release()
{
    SLresult result = (*this->playerInterface)->SetPlayState(this->playerInterface, SL_PLAYSTATE_STOPPED);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to stop player\n");
    (*this->player)->Destroy(this->player);
    this->player = NULL;
    this->effectSendInterface = NULL;
}

//------------------------------------------------------------------------------
/**
*/
SLPlayItf
SLESChannel::GetPlayerInterface() const
{
    return this->playerInterface;
}

//------------------------------------------------------------------------------
/**
*/
SLAndroidSimpleBufferQueueItf
SLESChannel::GetBufferQueueInterface() const
{
    return this->bufferQueueInterface;
}

//------------------------------------------------------------------------------
/**
*/
SLVolumeItf
SLESChannel::GetVolumeInterface() const
{
    return this->volumeInterface;
}

//------------------------------------------------------------------------------
/**
*/
SLEffectSendItf
SLESChannel::GetEffectSendInterface() const
{
    return this->effectSendInterface;
}

} // namespace chrissly