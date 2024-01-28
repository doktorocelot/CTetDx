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

    // Audio format
    WAVEFORMATEX waveFormat = {};

    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = 2;
    waveFormat.nSamplesPerSec = SAMPLE_RATE;
    waveFormat.wBitsPerSample = 16;
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
}
