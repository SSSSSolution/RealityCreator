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

#ifdef _WIN32
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

private:
    void createPresentationWindow(const int &windowWidth = 800, const int &windowHeight = 600);

public:
#ifdef _WIN32
#define APP_NAME_STR_LEN 80
    HINSTANCE connection;
//    wchar_t name[APP_NAME_STR_LEN];
    LPCWSTR name;
    HWND window;
#elif __linux__
    xcb_connection_t *connection;
    xcb_screen_t *screen;
    xcb_window_t window;
    xcb_intern_atom_reply_t *reply;
#endif

    uint32_t width, height;

private:
    VulkanSwapChain *vulkanSwapChain;
};

#endif // VULKANRENDERER_H
