#include "render/bmp.hpp"
#include "win32/win32_files.hpp"
#include "win32/win32_kill-program.hpp"

#include <windows.h>
#include <ctet/ctet.h>
#include "win32/win32_window.hpp"
#include "win32/win32_memory.hpp"

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdStr,
                     int showCmd) {
    Win32Window window{};
    const auto engine = static_cast<CTetEngine *>(win32_allocateMemory(
        ctEngine_getSize()));

    /*wchar_t filePath[MAX_PATH];

    win32_setCompleteFilePath(filePath, MAX_PATH, L"resources\\img\\skin.bmp");

    unsigned char *skinImgData = nullptr;
    BmpImage skinImage;

    if (win32_fileExists(filePath)) {
        HANDLE skinImgHandle = win32_openFile(filePath);
        skinImgData = win32_loadFileIntoNewVirtualBuffer(skinImgHandle);
        skinImage = bmp_init(
            skinImgData,
            static_cast<unsigned char *>(win32_allocateMemory(
                bmp_getBufferSize(skinImgData)
                ))
            );
        
    } else {
        win32_killProgram(L"Could not find block skin.");
    }*/

    ctEngine_init(engine);
    win32Window_init(&window, instance);
    win32Window_show(window.window);
    win32Window_loop(&window, engine);
    // Manually freeing memory and calling cleanup is pointless since the program is closing.
    // Windows will do the cleaning up. Don't waste the user's time.
    return 0;
}
