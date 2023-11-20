#include <iostream>
#include <xcb/xcb.h>

#include "die.hpp"

int main() {
    xcb_connection_t* connection = xcb_connect(nullptr, nullptr);

    if (xcb_connection_has_error(connection)) {
        die("Unable to open XCB connection.");
    }

    const xcb_setup_t* setup = xcb_get_setup(connection);
    xcb_screen_t* screen = xcb_setup_roots_iterator(setup).data;

    xcb_window_t window = xcb_generate_id(connection);
    xcb_create_window(
        connection,
        XCB_COPY_FROM_PARENT,
        window,
        screen->root,
        10, 10,
        720, 720,
        10,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        screen->root_visual,
        0, nullptr
    );

    xcb_map_window(connection, window);

    xcb_flush(connection);

    xcb_generic_event_t* event;
    while ((event = xcb_wait_for_event(connection))) {
        switch (event->response_type & ~0x80) {
            case XCB_CLIENT_MESSAGE:
                if (reinterpret_cast<xcb_client_message_event_t *>(event)->data.data32[0] == screen->root) {
                    free(event);
                    xcb_disconnect(connection);
                    return 0;
                }
                break;
        }
        free(event);
    }

    xcb_disconnect(connection);
}
