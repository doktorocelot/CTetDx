#pragma once
#include "../sound-pool.hpp"

#include <audioclient.h>
#include <mmdeviceapi.h>
#define NUM_AUDIO_CHANNELS 2
constexpr int FRAME_SIZE_BYTES = sizeof(float) * NUM_AUDIO_CHANNELS;
constexpr int AUDIO_SAMPLES_PER_UPDATE = 1024;

struct WasapiAudioSystem {
    IMMDeviceEnumerator *enumerator;
    IMMDevice *device;
    IAudioClient *audioClient;
    IAudioRenderClient *renderClient;
    IAudioClock *audioClock;
    unsigned int bufferSize;
    unsigned int sampleRate;
    unsigned long long pushedFrames;
};

void wasapiAudio_init(WasapiAudioSystem *system);

void wasapiAudio_process(WasapiAudioSystem *audioSystem, SoundPool *soundPool);