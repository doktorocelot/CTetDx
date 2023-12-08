#include <windows.h>
#include <ctet/ctet.h>
#include "win32/win32_window.hpp"
#include "win32/win32_memory.hpp"

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdStr, int showCmd) {
    Win32Window window{};
    const auto engine = static_cast<CTetEngine *>(win32_allocateMemory(ctEngine_getSize()));
    ctEngine_init(engine);
    win32Window_init(&window, instance);
    win32Window_show(window.window);
    win32Window_loop(&window, engine);
    // Manually freeing memory and calling cleanup is pointless since the program is closing.
    // Windows will do the cleaning up. Don't waste the user's time.
    return 0;
}
