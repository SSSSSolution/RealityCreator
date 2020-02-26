#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H

#include "headers.h"
#include "VulkanApplication.h"
#include "VulkanRenderer.h"

struct SwapChainBuffer {
    VkImage image;
    VkImageView view;
};

class VulkanSwapChain
{
public:
    VulkanSwapChain(VulkanRenderer *vulkanRenderer);
    void initialize();
    void createSwapChain(const VkCommandBuffer &cmd);

private:
    void querySwapChainExtensions();
    void createSurface();
    uint32_t getGraphicsQueueWithPresentationSupport();
    void getSupportedFormats();
    void getSurfaceCapabilitiesAndPresentMode();
    void managePresentMode();
    void createSwapChainColorImages();
    void createColorImageView(const VkCommandBuffer& cmd);

public:
    PFN_vkQueuePresentKHR vkQueuePresentKHR;
    PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;

private:
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;

    PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;

private:
    VulkanApplication *vulkanApplication;
    VulkanRenderer *vulkanRenderer;

public:
    VkSwapchainKHR swapChain;
    VkSurfaceFormatKHR surfaceFormat;
    VkFormat format;
    std::vector<SwapChainBuffer> colorBufferList;
    uint32_t currentColorBuffer;

private:
    VkSurfaceKHR surface;
    std::vector<VkSurfaceFormatKHR> surfaceFormatList;
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    uint32_t presentModeCount;
    std::vector<VkPresentModeKHR> presentModeList;
    VkPresentModeKHR swapchainPresentMode;
    VkExtent2D swapChainExtent;
    uint32_t desiredNumberOfSwapChainImages;
    VkSurfaceTransformFlagBitsKHR preTransform;
    uint32_t swapChainImagesCount;
    std::vector<VkImage> swapChainImages;
};

#endif // VULKANSWAPCHAIN_H
