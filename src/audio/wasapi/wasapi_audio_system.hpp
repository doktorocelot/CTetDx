#pragma once
#include <audioclient.h>
#include <mmdeviceapi.h>

struct WasapiAudioSystem {
    IMMDeviceEnumerator *enumerator;
    IMMDevice *device;
    IAudioClient *audioClient;
    IAudioRenderClient *renderClient;
    unsigned int bufferSize;
    unsigned int sampleRate;
};

void wasapiAudio_init(WasapiAudioSystem *system);