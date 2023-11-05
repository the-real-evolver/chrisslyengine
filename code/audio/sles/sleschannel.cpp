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
    effectSendInterface(NULL),
    requestRelease(false)
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
    const SLInterfaceID  ids[3U] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_EFFECTSEND};
    const SLboolean required[3U] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    SLresult result = (*engineInterface)->CreateAudioPlayer(engineInterface, &this->player, &audioSource, &audioSink, 3U, ids, required);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to create audio player\n");
    result = (*this->player)->Realize(this->player, SL_BOOLEAN_FALSE);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to realize audio player\n");

    result = (*this->player)->GetInterface(this->player, SL_IID_PLAY, &this->playerInterface);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to get play interface\n");

    result = (*this->player)->GetInterface(this->player, SL_IID_BUFFERQUEUE, &this->bufferQueueInterface);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to get bufferqueue interface\n");

    result = (*this->bufferQueueInterface)->RegisterCallback(this->bufferQueueInterface, SLESAudioRenderer::BufferQueueCallback, this);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to register callback\n");

    result = (*this->player)->GetInterface(this->player, SL_IID_VOLUME, &this->volumeInterface);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to get volume interface\n");

    SLmillibel maxVolume;
    result = (*this->volumeInterface)->GetMaxVolumeLevel(this->volumeInterface, &maxVolume);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to get max volume level\n");

    result = (*this->volumeInterface)->SetVolumeLevel(this->volumeInterface, this->mode & audio::MODE_3D ? SL_MILLIBEL_MIN : maxVolume);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to set volume\n");

    result = (*this->player)->GetInterface(this->player, SL_IID_EFFECTSEND, &this->effectSendInterface);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::SetupAudioPlayer(): failed to get effect send interface\n");
}

//------------------------------------------------------------------------------
/**
*/
void
SLESChannel::ReleaseAudioPlayer()
{
    if (this->playerInterface == NULL) return;
    SLresult result = (*this->playerInterface)->SetPlayState(this->playerInterface, SL_PLAYSTATE_STOPPED);
    CE_ASSERT(SL_RESULT_SUCCESS == result, "SLESChannel::Release(): failed to stop player\n");
    this->effectSendInterface = NULL;
    this->volumeInterface = NULL;
    this->bufferQueueInterface = NULL;
    this->playerInterface = NULL;
    (*this->player)->Destroy(this->player);
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

//------------------------------------------------------------------------------
/**
*/
void
SLESChannel::RequestRelease()
{
    this->requestRelease = true;
}

//------------------------------------------------------------------------------
/**
*/
bool
SLESChannel::GetReleaseRequest()
{
    bool requestRelease = this->requestRelease;
    this->requestRelease = false;
    return requestRelease;
}

} // namespace chrissly
