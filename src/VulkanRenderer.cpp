#include "VulkanRenderer.h"
#include "VulkanSwapChain.h"
#include "VulkanApplication.h"

VulkanRenderer::VulkanRenderer()
{
    vulkanSwapChain = new VulkanSwapChain(this);
}

void VulkanRenderer::initialize()
{
    createPresentationWindow();
    vulkanSwapChain->initialize();

}

#ifdef _WIN32
// MS-Windows event handling function:
LRESULT CALLBACK VulkanRenderer::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    qDebug() << __func__;
//    switch (uMsg)
//    {
//    case WM_CLOSE:
//        PostQuitMessage(0);
//        break;
//    case WM_PAINT:
//        for each (VulkanDrawable* drawableObj in appObj->rendererObj->drawableList)
//        {
//            drawableObj->render();
//        }

//        return 0;
//    default:
//        break;
//    }
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void VulkanRenderer::createPresentationWindow(const int& windowWidth, const int& windowHeight)
{
    assert(windowWidth > 0 || windowHeight > 0);

    WNDCLASSEX  winInfo;

    name = L"Drawing Hello World";
    memset(&winInfo, 0, sizeof(WNDCLASSEX));
    // Initialize the window class structure:
    winInfo.cbSize			= sizeof(WNDCLASSEX);
    winInfo.style			= CS_HREDRAW | CS_VREDRAW;
    winInfo.lpfnWndProc		= WndProc;
    winInfo.cbClsExtra		= 0;
    winInfo.cbWndExtra		= 0;
    winInfo.hInstance		= connection; // hInstance
    winInfo.hIcon			= LoadIcon(nullptr, IDI_APPLICATION);
    winInfo.hCursor			= LoadCursor(nullptr, IDC_ARROW);
    winInfo.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
    winInfo.lpszMenuName	= nullptr;
    winInfo.lpszClassName	= name;
    winInfo.hIconSm			= LoadIcon(nullptr, IDI_WINLOGO);

    // Register window class:
    if (!RegisterClassEx(&winInfo)) {
        // It didn't work, so try to give a useful error:
        printf("Unexpected error trying to start the application!\n");
        fflush(stdout);
        exit(1);
    }

    // Create window with the registered class:
    RECT wr = { 0, 0, windowWidth, windowHeight };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    window = CreateWindowEx(0,
                            name,					// class name
                            name,					// app name
                            WS_OVERLAPPEDWINDOW |	// window style
                            WS_VISIBLE |
                            WS_SYSMENU,
                            100, 100,				// x/y coords
                            wr.right - wr.left,     // width
                            wr.bottom - wr.top,     // height
                            nullptr,					// handle to parent
                            nullptr,					// handle to menu
                            connection,				// hInstance
                            nullptr);					// no extra parameters

    if (!window) {
        // It didn't work, so try to give a useful error:
        printf("Cannot create a window in which to draw!\n");
        fflush(stdout);
        exit(1);
    }

    SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)(VulkanApplication::getInstance()));
}
#elif __linux__
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
#endif // _WIN32

































