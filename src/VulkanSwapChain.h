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

private:
    void querySwapChainExtensions();
    void createSurface();

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

private:
    VkSurfaceKHR surface;

};

#endif // VULKANSWAPCHAIN_H
