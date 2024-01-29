#include "wasapi_audio_system.hpp"

#include "../../win32/win32_check-result.hpp"
#include "../../win32/win32_kill-program.hpp"

#include <windows.h>

void wasapiAudio_init(WasapiAudioSystem *system) {
    HRESULT result;

    // create enumator for audio devices
    result = CoCreateInstance(__uuidof(MMDeviceEnumerator),
                              nullptr,
                              CLSCTX_ALL,
                              __uuidof(IMMDeviceEnumerator),
                              reinterpret_cast<void **>(&system->enumerator));
    win32_checkResult(result, "CoCreateInstance (MMDeviceEnumerator)");

    // Get the default audio device
    result = system->enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &system->device);
    win32_checkResult(result, "GetDefaultAudioEndpoint");

    // Active audio client
    result = system->device->Activate(__uuidof(IAudioClient),
                              CLSCTX_ALL,
                              nullptr,
                              reinterpret_cast<void **>(&system->audioClient));
    win32_checkResult(result, "Activate (Device -> AudioClient)");

    WAVEFORMATEX *deviceFormat;
    result = system->audioClient->GetMixFormat(&deviceFormat);
    win32_checkResult(result, "Audio Client GetMIxFormat)");

    system->sampleRate = deviceFormat->nSamplesPerSec;
    CoTaskMemFree(deviceFormat);
    
    // Audio format
    WAVEFORMATEX waveFormat = {};

    waveFormat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    waveFormat.nChannels = 2;
    waveFormat.nSamplesPerSec = system->sampleRate;
    waveFormat.wBitsPerSample = 32;
    waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample /
                             8;
    waveFormat.nAvgBytesPerSec =
        waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

    // Init audio stream
    result = system->audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                     0,
                                     30000000,
                                     0,
                                     &waveFormat,
                                     nullptr);
    win32_checkResult(result, "AudioClient Initalize");
    
    result = system->audioClient->GetBufferSize(&system->bufferSize);
    win32_checkResult(result, "AudioClient GetBufferSize");

    // Create an event handle for the audio client
    HANDLE event = CreateEvent(nullptr, false, false, nullptr);
    if (event == nullptr) {
        win32_killProgram(
            L"Unable to create an event handle for the audio client.");
    }

    // Get the render client
    result = system->audioClient->GetService(__uuidof(IAudioRenderClient),
                                     reinterpret_cast<void **>(&system->renderClient));
    win32_checkResult(result, "AudioClient GetService (RenderClient)");
    
    result = system->audioClient->Start();
    win32_checkResult(result, "AudioClient Start");

    unsigned char *data;

    result = system->audioClient->GetService(__uuidof(IAudioClock),
        reinterpret_cast<void **>(&system->audioClock));
    win32_checkResult(result, "AudioClient GetService (AudioClock)");
    

    // // Preload audio data to keep a gap between playing and the buffer
    const unsigned int preloadFrames = system->sampleRate / 50;
    result = system->renderClient->GetBuffer(preloadFrames, &data);
    win32_checkResult(result, "Audio RenderClient -> GetBuffer");

    system->pushedFrames += preloadFrames;
    
    result = system->renderClient->ReleaseBuffer(preloadFrames, 0);
    win32_checkResult(result, "Audio RenderClient -> ReleaseBuffer");
}
