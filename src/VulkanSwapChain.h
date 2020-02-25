#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H

#include "headers.h"
#include "VulkanApplication.h"
#include "VulkanRenderer.h"

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
    VkSurfaceFormatKHR surfaceFormat;
private:
    VkSurfaceKHR surface;
    std::vector<VkSurfaceFormatKHR> surfaceFormatList;
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    uint32_t presentModeCount;
    std::vector<VkPresentModeKHR> presentModeList;
    VkPresentModeKHR swapchainPresentMode;
    VkExtent2D swapChainExtent;

};

#endif // VULKANSWAPCHAIN_H
