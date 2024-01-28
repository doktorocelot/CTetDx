#pragma once
#include <audioclient.h>
#include <mmdeviceapi.h>

#define SAMPLE_RATE 44100

struct WasapiAudioSystem {
    IMMDeviceEnumerator *enumerator;
    IMMDevice *device;
    IAudioClient *audioClient;
    IAudioRenderClient *renderClient;
    unsigned int bufferSize;
};

void wasapiAudio_init(WasapiAudioSystem *system);