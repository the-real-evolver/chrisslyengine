//------------------------------------------------------------------------------
//  wasapiaudiorenderer.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "wasapiaudiorenderer.h"
#include "audiosystem.h"
#include "dsp/samplerateconverter.h"
#include "dsp/mixer.h"
#include "miscutils.h"
#include <functiondiscoverykeys_devpkey.h>

namespace chrissly
{

WASAPIAudioRenderer* WASAPIAudioRenderer::Singleton = NULL;

static const double RequestedFrameDurationInSeconds = 0.023;
static const unsigned short AudioChannelMax = 8U;
static const REFERENCE_TIME ReftimesPerSec = 10000000;

//------------------------------------------------------------------------------
/**
*/
DWORD WINAPI
ThreadProc(_In_ LPVOID lpParameter)
{
    WASAPIAudioRenderer* renderer = (WASAPIAudioRenderer*)lpParameter;
    renderer->RunAudioThread();
    return 0U;
}

//------------------------------------------------------------------------------
/**
*/
WASAPIAudioRenderer::WASAPIAudioRenderer() :
    running(false),
    thread(NULL),
    enumerator(NULL),
    device(NULL),
    audioClient(NULL),
    renderClient(NULL),
    bufferFrameCount(0U)
{
    Singleton = this;
    memset(&this->deviceFormat, 0, sizeof(this->deviceFormat));
    memset(this->resampleBuffer, 0, sizeof(this->resampleBuffer));
    memset(this->channelMixBuffer, 0, sizeof(this->channelMixBuffer));
}

//------------------------------------------------------------------------------
/**
*/
WASAPIAudioRenderer::~WASAPIAudioRenderer()
{
    Singleton = NULL;
}

//------------------------------------------------------------------------------
/**
*/
void
WASAPIAudioRenderer::Initialise(void* const customParams)
{
    CE_UNREFERENCED_PARAMETER(customParams);

    HRESULT result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to initialise COM library for use by the calling thread\n");

    result = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&this->enumerator);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to device enumerator\n");

    result = this->enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &this->device);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to get audio endpoint\n");

    IPropertyStore* props = NULL;
    result = this->device->OpenPropertyStore(STGM_READ, &props);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to open property store\n");
    PROPVARIANT prop;
    PropVariantInit(&prop);
    result = props->GetValue(PKEY_Device_FriendlyName, &prop);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to get audio device name\n");
    CE_LOG("\nWASAPIAudioRenderer::Initialise(): default audio endpoint \"%S\"\n", prop.pwszVal);
    PropVariantClear(&prop);
    props->Release();

    result = this->device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&this->audioClient);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to activate audio client\n");

    WAVEFORMATEX* format = NULL;
    result = this->audioClient->GetMixFormat(&format);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to get mix format\n");
    memcpy(&this->deviceFormat, format, sizeof(this->deviceFormat));
    format->wFormatTag = WAVE_FORMAT_PCM;
    format->nChannels = 2U;
    format->wBitsPerSample = 16U;
    format->nBlockAlign = 4U;
    format->nAvgBytesPerSec = format->nSamplesPerSec * format->nBlockAlign;
    format->cbSize = 0U;

    REFERENCE_TIME hnsRequestedDuration = (REFERENCE_TIME)(RequestedFrameDurationInSeconds * (double)ReftimesPerSec);
    result = this->audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0U, hnsRequestedDuration, 0, format, NULL);
    if (FAILED(result))
    {
        format->nChannels = this->deviceFormat.nChannels;
        format->wBitsPerSample = this->deviceFormat.wBitsPerSample;
        result = this->audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0U, hnsRequestedDuration, 0, format, NULL);
        CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to initialise audio client\n");
    }
    else
    {
        this->deviceFormat.nChannels = 2U;
        this->deviceFormat.wBitsPerSample = 16U;
    }

    result = this->audioClient->GetBufferSize(&this->bufferFrameCount);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to get buffer size\n");

    result = this->audioClient->GetService(__uuidof(IAudioRenderClient), (void**)&this->renderClient);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to get service\n");

    result = this->audioClient->Start();
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to start audio client\n");

    CoTaskMemFree(format);
}

//------------------------------------------------------------------------------
/**
*/
void
WASAPIAudioRenderer::Shutdown()
{
    if (this->audioClient != NULL)
    {
#if __CE_DEBUG__
        HRESULT result =
#endif
        this->audioClient->Stop();
        CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Shutdown(): failed to stop audio client\n");
        this->audioClient->Release();
        this->audioClient = NULL;
    }
    if (this->renderClient != NULL)
    {
        this->renderClient->Release();
        this->renderClient = NULL;
    }
    if (this->device != NULL)
    {
        this->device->Release();
        this->device = NULL;
    }
    if (this->enumerator != NULL)
    {
        this->enumerator->Release();
        this->enumerator = NULL;
    }

    CoUninitialize();
}

//------------------------------------------------------------------------------
/**
*/
void
WASAPIAudioRenderer::StartAudioProcessing()
{
    this->running = true;

    this->thread = CreateThread(NULL, 0U, ThreadProc, (LPVOID)this, 0U, NULL);
    CE_ASSERT(this->thread != NULL, "WASAPIAudioRenderer::StartAudioProcessing(): failed to create audio thread\n");
#if __CE_DEBUG__
    BOOL success =
#endif
    SetThreadPriority(this->thread, THREAD_PRIORITY_HIGHEST);
    CE_ASSERT(success != FALSE, "WASAPIAudioRenderer::StartAudioProcessing(): failed to set thread priority\n");
}

//------------------------------------------------------------------------------
/**
*/
void
WASAPIAudioRenderer::StopAudioProcessing()
{
    this->running = false;

#if __CE_DEBUG__
    DWORD state =
#endif
    WaitForSingleObject(this->thread, INFINITE);
    CE_ASSERT(WAIT_OBJECT_0 == state, "WASAPIAudioRenderer::StopAudioProcessing(): failed to wait for thread '%p'\n", this->thread);
#if __CE_DEBUG__
    BOOL result =
#endif
    CloseHandle(this->thread);
    CE_ASSERT(result != FALSE, "WASAPIAudioRenderer::StopAudioProcessing(): failed to close thread handle '%p'\n", this->thread);
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
WASAPIAudioRenderer::GetNumHardwareChannels() const
{
    return AudioChannelMax;
}

//------------------------------------------------------------------------------
/**
*/
void
WASAPIAudioRenderer::StartChannel(audio::Channel* const channel)
{
    this->syncLock.Lock();
    audio::Sound* sound;
    channel->GetCurrentSound(&sound);
    sound->IncrementUseCount();
    channel->SetIndex(0);
    channel->SetIsPlaying(true);
    this->syncLock.Unlock();
}

//------------------------------------------------------------------------------
/**
*/
void
WASAPIAudioRenderer::UpdateChannel(audio::Channel* const channel)
{
    CE_UNREFERENCED_PARAMETER(channel);
}

//------------------------------------------------------------------------------
/**
*/
void
WASAPIAudioRenderer::ReleaseChannel(audio::Channel* const channel)
{
    this->syncLock.Lock();
    channel->ReleaseInternal();
    this->syncLock.Unlock();
}

//------------------------------------------------------------------------------
/**
*/
void
WASAPIAudioRenderer::RunAudioThread()
{
    HRESULT result;
    UINT32 numFramesPadding = 0U, numFramesAvailable = 0U;
    BYTE* data = NULL;
    DWORD flags = 0U;

    while (this->running)
    {
        result = this->audioClient->GetCurrentPadding(&numFramesPadding);
        CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::RunAudioThread(): failed to get current padding\n");

        numFramesAvailable = this->bufferFrameCount - numFramesPadding;

        if (numFramesAvailable > 0U)
        {
            this->syncLock.Lock();

            result = this->renderClient->GetBuffer(numFramesAvailable, &data);
            CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::RunAudioThread(): failed to get buffer\n");

            if (data != NULL)
            {
                // resample
                if (this->deviceFormat.nSamplesPerSec != 44100U)
                {
                    unsigned int numSamplesToMix = (unsigned int)((double)numFramesAvailable * (44100.0 / (double)this->deviceFormat.nSamplesPerSec));
                    audio::AudioSystem::Instance()->_Mix(numSamplesToMix, (unsigned char*)this->resampleBuffer);
                    ce_audio_resample_s16_stereo(this->resampleBuffer, numSamplesToMix, this->channelMixBuffer, numFramesAvailable);
                }
                else
                {
                    audio::AudioSystem::Instance()->_Mix(numFramesAvailable, (unsigned char*)this->channelMixBuffer);
                }

                // down-mix or up-mix
                switch (this->deviceFormat.nChannels)
                {
                    case 1U:
                        switch (this->deviceFormat.wBitsPerSample)
                        {
                            case 32U:
                                ce_audio_downmix_s16_stereo_s32_mono((short*)this->channelMixBuffer, (int*)data, numFramesAvailable);
                                break;
                            default:
                                break;
                        }

                        break;
                    case 2U:
                        switch (this->deviceFormat.wBitsPerSample)
                        {
                            case 16U:
                                memcpy(data, this->channelMixBuffer, numFramesAvailable * 4U);
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
            }

            result = this->renderClient->ReleaseBuffer(numFramesAvailable, flags);
            CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::RunAudioThread(): failed to release buffer\n");

            this->syncLock.Unlock();
        }
    }
}

} // namespace chrissly
