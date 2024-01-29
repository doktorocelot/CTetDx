#include "sound-bank.hpp"

#include "win32/win32_audio.hpp"

#include <string>

static const wchar_t *FILE_NAMES[] = {
    L"hard-drop",
    L"shift",
    L"lock",
    L"rotate",
    L"hold",
    L"erase1",
    L"erase2",
    L"erase3",
    L"erase4",
    L"collapse",
    L"buffer-rotate",
    L"buffer-hold",
};

void loadSounds(PcmS16Buffer soundBuffers[Sounds_LENGTH]) {
    for (int i = 0; i < Sounds_LENGTH; i++) {
        std::wstring path{};
        path += L"resources\\se\\";
        path += FILE_NAMES[i];
        path += L".wav";
        soundBuffers[i] = win32_loadAudioIntoBuffer(path.c_str());
    }
}
