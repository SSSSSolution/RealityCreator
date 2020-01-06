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
    // store the image surface capabilities
    VkSurfaceCapabilitiesKHR surfCapabilities;

    // store the number of present mode support by the implementation
    uint32_t presentModeCount;

    // Arrays for retrived present modes
    std::vector<VkPresentModeKHR> presentModes;

    // size of the swap color images
    VkExtent2D swapChainExtent;

    // Number of color images supported by the implementation
    uint32_t desiredNumberOfSwapChainImages;

    VkSurfaceTransformFlagBitsKHR preTransform;

    // Stores present mode bitwise flag for the creation of swap chain
    VkPresentModeKHR swapChainPresentMode;

    // The retrived drawing color swap chain images
    std::vector<VkImage> swapChainImages;

    std::vector<VkSurfaceFormatKHR> surfFormats;
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
    VulkanSwapChain(VulkanInstance *inst, VulkanDevice *device, VkSurfaceKHR surface);
    ~VulkanSwapChain() {}

    void intializeSwapChain();
    void createSwapChain(const VkCommandBuffer &cmd);
    void destroySwapChain(){}

private:
    void createSwapChainExtensions();
    void getSurfaceCapabilitiesAndPresentMode();
    void managePresentMode();

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

    SwapChainPrivateVariables scPrivateVars;
};

#endif // VULKANSWAPCHAIN_H
