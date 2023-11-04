#include <windows.h>
#include "window/window.hpp"

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdStr, int showCmd) {
    Window window{};
    window_init(&window, instance);
    window_show(window.window);
    window_loop(&window);
    window_cleanup(&window);
}
