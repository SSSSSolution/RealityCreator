#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include "headers.h"

class VulkanSwapChain;
class VulkanApplication;
class VulkanDevice;

#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT
class VulkanRenderer
{
public:
    VulkanRenderer();

public:
    void initialize();
//    bool prepare();
//    bool render();


    void createCommandPool();
    void buildSwapChainAndDepthImage();
    void createDepthImage();

#ifdef _WIN32
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

private:
    void createPresentationWindow(const int &windowWidth = 800, const int &windowHeight = 600);

    void setImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout,
                        VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask, const VkCommandBuffer &cmdBuf);

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

    struct {
        VkFormat format;
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    } Depth;

    uint32_t width, height;

private:
    VulkanApplication *vulkanApplication;
    VulkanDevice *vulkanDevice;
    VulkanSwapChain *vulkanSwapChain;

    VkCommandBuffer cmdDepthImage;
    VkCommandPool cmdPool;

};

#endif // VULKANRENDERER_H
