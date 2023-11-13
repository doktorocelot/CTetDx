#include <windows.h>
#include "window/window.hpp"
extern "C" {
#include "ctet/engine.h"
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdStr, int showCmd) {
    Window window{};
    auto engine = engine_create();
    window_init(&window, instance);
    window_show(window.window);
    window_loop(&window, engine);
    engine_destroy(engine);
    window_cleanup(&window);
    
    return 0;
}
