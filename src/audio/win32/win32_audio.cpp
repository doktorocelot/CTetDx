#include "win32_audio.hpp"
#include "windows.h"
#include "../../win32/win32_files.hpp"
#include "../../win32/win32_kill-program.hpp"
#include "../../win32/win32_memory.hpp"

#include <string>

PcmS16Buffer win32_loadAudioIntoBuffer(const wchar_t *path) {
    wchar_t filePath[MAX_PATH];
    win32_setCompleteFilePath(filePath, MAX_PATH, path);

    PcmS16Buffer pcmData = {};
    
    if (win32_fileExists(filePath)) {
        const HANDLE audioFileHandle = win32_openFile(filePath);
        unsigned char *audioFileData = win32_loadFileIntoNewVirtualBuffer(
            audioFileHandle);
        win32_closeFile(audioFileHandle);
        const size_t size = wav_getAudioSize(audioFileData);
        pcmData = wav_init(
            audioFileData,
            static_cast<unsigned char *>(win32_allocateMemory(
                wav_getAudioSize(audioFileData)
            )),
            size
        );
        
        win32_deallocateMemory(audioFileData);
    } else {
        const std::wstring errorMessage = L"Could not load static texture " + std::wstring(path);
        win32_killProgram(errorMessage.c_str());
    }

    return pcmData;
}
