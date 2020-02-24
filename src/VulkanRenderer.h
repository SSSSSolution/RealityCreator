#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include "headers.h"

class VulkanSwapChain;

class VulkanRenderer
{
public:
    VulkanRenderer();

public:
    void initialize();
//    bool prepare();
//    bool render();

private:
    void createPresentationWindow(const int &windowWidth = 800, const int &windowHeight = 600);

private:
#ifdef _WIN32
#define APP_NAME_STR_LEN 80
    HINSTANCE connectin;
    char name[APP_NAME_STR_LEN];
    HWND window;
#elif __linux__
    xcb_connection_t *connection;
    xcb_screen_t *screen;
    xcb_window_t window;
    xcb_intern_atom_reply_t *reply;
#endif

private:
    VulkanSwapChain *vulkanSwapChain;
};

#endif // VULKANRENDERER_H
