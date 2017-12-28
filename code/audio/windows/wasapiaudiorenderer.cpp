//------------------------------------------------------------------------------
//  wasapiaudiorenderer.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "wasapiaudiorenderer.h"
#include "audiosystem.h"
#include "dsp/samplerateconverter.h"

namespace chrissly
{

WASAPIAudioRenderer* WASAPIAudioRenderer::Singleton = NULL;

static const unsigned int RequestedBufferSizeInSamples = 1024U;
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
    bufferFrameCount(0U),
    resample(false),
    resampleRatio(1.0f)
{
    Singleton = this;
    memset(this->resampleBuffer, 0, sizeof(this->resampleBuffer));
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

    result = this->device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&this->audioClient);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to activate audio client\n");

    WAVEFORMATEX* format = NULL;
    result = this->audioClient->GetMixFormat(&format);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to get mix format\n");
    format->wFormatTag = WAVE_FORMAT_PCM;
    format->nChannels = 2U;
    format->wBitsPerSample = 16U;
    format->nBlockAlign = 4U;
    format->nAvgBytesPerSec = format->nSamplesPerSec * format->nBlockAlign;
    format->cbSize = 0U;

    REFERENCE_TIME hnsRequestedDuration = (REFERENCE_TIME)((double)RequestedBufferSizeInSamples / (double)format->nSamplesPerSec * (double)ReftimesPerSec);
    result = this->audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0U, hnsRequestedDuration, 0, format, NULL);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to initialise audio client\n");

    result = this->audioClient->GetBufferSize(&this->bufferFrameCount);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to get buffer size\n");

    result = this->audioClient->GetService(__uuidof(IAudioRenderClient), (void**)&this->renderClient);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to get service\n");

    result = this->audioClient->Start();
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Initialise(): failed to start audio client\n");

    if (format->nSamplesPerSec != 44100U)
    {
        CE_ASSERT(format->nSamplesPerSec <= 88200U, "WASAPIAudioRenderer::Initialise(): upsampling to %i Hz not supported by the samplerate converter\n");
        this->resample = true;
        this->resampleRatio = 44100.0 / (double)format->nSamplesPerSec;
    }

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
#if __DEBUG__
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
#if __DEBUG__
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

#if __DEBUG__
    DWORD state =
#endif
    WaitForSingleObject(this->thread, INFINITE);
    CE_ASSERT(WAIT_OBJECT_0 == state, "WASAPIAudioRenderer::StopAudioProcessing(): failed to wait for thread '%p'\n", this->thread);
#if __DEBUG__
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
    sound->_IncrementUseCount();
    channel->_SetIndex(0);
    channel->_SetIsPlaying(true);
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
    channel->_Release();
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
                if (this->resample)
                {
                    unsigned int numSamplesToMix = (unsigned int)((double)numFramesAvailable * this->resampleRatio);
                    audio::AudioSystem::Instance()->_Mix(numSamplesToMix, (unsigned char*)this->resampleBuffer);
                    ce_audio_resample_s16_stereo(this->resampleBuffer, numSamplesToMix, (int*)data, numFramesAvailable);
                }
                else
                {
                    audio::AudioSystem::Instance()->_Mix(numFramesAvailable, data);
                }
            }

            result = this->renderClient->ReleaseBuffer(numFramesAvailable, flags);
            CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::RunAudioThread(): failed to release buffer\n");

            this->syncLock.Unlock();
        }
    }
}

} // namespace chrissly
