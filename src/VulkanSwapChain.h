#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H

#include "headers.h"

class VulkanSwapChain
{
public:
    VulkanSwapChain();
    void initialize();

private:
    void querySwapChainExtensions();


private:
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;

    PFN_vkCreateSwapchainKHR vkVreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;

};

#endif // VULKANSWAPCHAIN_H
