//------------------------------------------------------------------------------
//  wasapiaudiorenderer.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "wasapiaudiorenderer.h"
#include "audiosystem.h"

namespace chrissly
{

WASAPIAudioRenderer* WASAPIAudioRenderer::Singleton = NULL;

static const unsigned int RequestedBufferSizeInSamples = 1024;
static const unsigned short AudioChannelMax = 8;
static const REFERENCE_TIME ReftimesPerSec = 10000000;

//------------------------------------------------------------------------------
/**
*/
DWORD WINAPI
ThreadProc(_In_ LPVOID lpParameter)
{
    WASAPIAudioRenderer* renderer = (WASAPIAudioRenderer*)lpParameter;
    renderer->RunAudioThread();
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
WASAPIAudioRenderer::WASAPIAudioRenderer() :
    running(false),
    thread(0),
    enumerator(NULL),
    device(NULL),
    audioClient(NULL),
    renderClient(NULL),
    bufferFrameCount(0)
{
    Singleton = this;
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
WASAPIAudioRenderer::_Initialise(void* customParams)
{
    CE_UNREFERENCED_PARAMETER(customParams);

    HRESULT result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::_Initialise() failed to initialise COM library for use by the calling thread\n");

    result = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&this->enumerator);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::_Initialise() failed to device enumerator\n");

    result = this->enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &this->device);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::_Initialise() failed to get audio endpoint\n");

    result = this->device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&this->audioClient);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::_Initialise() failed to activate audio client\n");

    WAVEFORMATEX* format = NULL;
    result = this->audioClient->GetMixFormat(&format);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::_Initialise() failed to get mix format\n");
    format->wFormatTag = WAVE_FORMAT_PCM;
    format->nChannels = 2;
    format->wBitsPerSample = 16;
    format->nBlockAlign = 4;
    format->nAvgBytesPerSec = format->nSamplesPerSec * format->nBlockAlign;
    format->cbSize = 0;

    REFERENCE_TIME hnsRequestedDuration = (REFERENCE_TIME)((double)RequestedBufferSizeInSamples / (double)format->nSamplesPerSec * (double)ReftimesPerSec);
    result = this->audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, format, NULL);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::_Initialise() failed to initialize audio client\n");

    result = this->audioClient->GetBufferSize(&this->bufferFrameCount);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::_Initialise() failed to get buffer size\n");

    result = this->audioClient->GetService(__uuidof(IAudioRenderClient), (void**)&this->renderClient);
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::_Initialise() failed to get service\n");

    result = this->audioClient->Start();
    CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::_Initialise() failed to start audio client\n");

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
        CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::Shutdown() failed to stop audio client\n");
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

    this->thread = CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, NULL);
    CE_ASSERT(this->thread != 0, "WASAPIAudioRenderer::StartAudioProcessing(): failed to create audio thread\n");
#if __DEBUG__
    BOOL success =
#endif
    SetThreadPriority(this->thread, THREAD_PRIORITY_HIGHEST);
    CE_ASSERT(success != 0, "WASAPIAudioRenderer::StartAudioProcessing(): failed to set thread priority\n");
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
    CE_ASSERT(result != 0, "WASAPIAudioRenderer::StopAudioProcessing(): failed to close thread handle '%p'\n", this->thread);
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
WASAPIAudioRenderer::StartChannel(audio::Channel* channel)
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
WASAPIAudioRenderer::UpdateChannel(audio::Channel* channel)
{
    CE_UNREFERENCED_PARAMETER(channel);
}

//------------------------------------------------------------------------------
/**
*/
void
WASAPIAudioRenderer::ReleaseChannel(audio::Channel* channel)
{
    this->syncLock.Lock();
    channel->_SetIsPlaying(false);
    channel->_SetIndex(audio::Channel::CHANNEL_FREE);
    audio::Sound* sound;
    channel->GetCurrentSound(&sound);
    sound->_DecrementUseCount();
    this->syncLock.Unlock();
}

//------------------------------------------------------------------------------
/**
*/
void
WASAPIAudioRenderer::RunAudioThread()
{
    HRESULT result;
    UINT32 numFramesPadding = 0, numFramesAvailable = 0;
    BYTE* data = NULL;
    DWORD flags = 0;

    while (this->running)
    {
        this->syncLock.Lock();

        result = this->audioClient->GetCurrentPadding(&numFramesPadding);
        CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::RunAudioThread() failed to get current padding\n");

        numFramesAvailable = this->bufferFrameCount - numFramesPadding;
        result = this->renderClient->GetBuffer(numFramesAvailable, &data);
        CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::RunAudioThread() failed to get buffer\n");

        if (numFramesAvailable > 0 && data != NULL)
        {
            audio::AudioSystem::Instance()->Mix(numFramesAvailable, data);
        }

        result = this->renderClient->ReleaseBuffer(numFramesAvailable, flags);
        CE_ASSERT(SUCCEEDED(result), "WASAPIAudioRenderer::RunAudioThread() failed to release buffer\n");

        this->syncLock.Unlock();
    }
}

} // namespace chrissly
