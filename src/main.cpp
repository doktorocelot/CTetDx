#include <windows.h>
#include <ctet/ctet.h>
#include "window/window.hpp"

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdStr, int showCmd) {
    Window window{};
    auto engine = ctEngine_create();
    window_init(&window, instance);
    window_show(window.window);
    window_loop(&window, engine);
    ctEngine_destroy(engine);
    window_cleanup(&window);
    
    return 0;
}
