#include <windows.h>
#include <ctet/ctet.h>
#include "win32/win32_window.hpp"

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdStr, int showCmd) {
    Win32Window window{};
    const auto engine = ctEngine_create();
    win32Window_init(&window, instance);
    win32Window_show(window.window);
    win32Window_loop(&window, engine);
    ctEngine_destroy(engine);
    win32Window_cleanup(&window);
    
    return 0;
}
