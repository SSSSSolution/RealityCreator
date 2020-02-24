#include "VulkanRenderer.h"
#include "VulkanSwapChain.h"

VulkanRenderer::VulkanRenderer()
{
    vulkanSwapChain = new VulkanSwapChain();
}

void VulkanRenderer::initialize()
{
    createPresentationWindow();
    vulkanSwapChain->initialize();

}


/* Magic code to me until now. */
void VulkanRenderer::createPresentationWindow(const int &windowWidth, const int &windowHeight)
{
    qDebug() << __func__;
    assert(windowWidth > 0);
    assert(windowHeight > 0);

    const xcb_setup_t *setup;
    xcb_screen_iterator_t iter;
    int scr;

    connection = xcb_connect(NULL, &scr);
    if (connection == NULL) {
        std::cout << "Cannot find a compatible Vulkan ICD.\n";
        exit(-1);
    }

    setup = xcb_get_setup(connection);
    iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0)
        xcb_screen_next(&iter);

    screen = iter.data;
    //

    uint32_t value_mask, value_list[32];

    window = xcb_generate_id(connection);
    qDebug() << "window id: " << window;

    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = screen->black_pixel;
    value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE;

    xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0, windowWidth, windowHeight, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, value_mask, value_list);

    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, cookie, 0);

    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(connection, 0, 16, "WM_DELETE_WINDOW");
    reply = xcb_intern_atom_reply(connection, cookie2, 0);

    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, (*reply).atom, 4, 32, 1, &(*reply).atom);
    qDebug() << "---------";
    free(reply);


    xcb_map_window(connection, window);

    const uint32_t coords[] = { 100, 100 };
    xcb_configure_window(connection, window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
    xcb_flush(connection);

    qDebug() << "---------";
    xcb_generic_event_t *e;
    while((e = xcb_wait_for_event(connection))) {
        if ((e->response_type & ~0x80) == XCB_EXPOSE)
            break;
    }
}


































