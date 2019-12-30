#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H

#include <vulkan/vulkan.h>
#include "VulkanInstance.h"
#include "VulkanDevice.h"

struct SwapChainBuffer {
    VkImage image;
    VkImageView view;
};

struct SwapChainPrivateVariables
{
    VkSurfaceCapabilitiesKHR surfCapabilities;

    uint32_t presentModeCount;
};

struct SwapChainPublicVariables
{
    // The logical platform dependent surface object
    VkSurfaceKHR surface;

    // Number of buffer image used for swap chain
    uint32_t swapchainImageCount;

    // Swap chain object
    VkSwapchainKHR swapChain;

    // List of color swap chain images
    std::vector<SwapChainBuffer> colorBuffer;

    // Semaphore for sync purpose
    VkSemaphore presentCompleteSemaphore;

    // current drawing surface index in use
    uint32_t currentColorBuffer;

    // format of the image
    VkFormat format;
};

class VulkanSwapChain
{
public:
    VulkanSwapChain(VulkanInstance *inst, VulkanDevice *device);
    ~VulkanSwapChain() {}

    void intializeSwapChain();
    void createSwapChain(const VkCommandBuffer &cmd);
    void destroySwapChain(){}

private:
    void createSwapChainExtensions();
    VkResult createSurface();
    void getSurfaceCapabilitiesAndPresentMode();

public:
    SwapChainPublicVariables scPublicVars;
    PFN_vkQueuePresentKHR fpQueuePresentKHR;
    PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;

private:
    VulkanInstance *m_instance;
    VulkanDevice *m_device;

    PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkDestroySurfaceKHR fpDestroySurfaceKHR;

    // layer extensions debugging
    PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;

//    SwapChainPrivateVaria
};

#endif // VULKANSWAPCHAIN_H
