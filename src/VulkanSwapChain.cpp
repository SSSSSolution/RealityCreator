#include "VulkanSwapChain.h"
#include "VulkanApplication.h"

#define GET_INSTANCE_FUNC_PTR(instance, funcname) {                                  \
    funcname = (PFN_##funcname)vkGetInstanceProcAddr(instance, #funcname);       \
    assert(funcname != nullptr);                                                \
}

#define GET_DEVICE_FUNC_PTR(dev, funcname) { \
    funcname = (PFN_##funcname)vkGetDeviceProcAddr(dev, #funcname); \
    assert(funcname != nullptr); \
}

VulkanSwapChain::VulkanSwapChain()
{
}

void VulkanSwapChain::initialize()
{
    querySwapChainExtensions();
}

void VulkanSwapChain::querySwapChainExtensions()
{
    VkInstance instance = VulkanApplication::getInstance()->vulkanInstance.vkInstance;
    VkDevice device = VulkanApplication::getInstance()->vulkanDevice.vkDevice;

    GET_INSTANCE_FUNC_PTR(instance, vkGetPhysicalDeviceSurfaceSupportKHR);
    GET_INSTANCE_FUNC_PTR(instance, vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    GET_INSTANCE_FUNC_PTR(instance, vkGetPhysicalDeviceSurfaceFormatsKHR);
    GET_INSTANCE_FUNC_PTR(instance, vkGetPhysicalDeviceSurfacePresentModesKHR);
    GET_INSTANCE_FUNC_PTR(instance, vkDestroySurfaceKHR);

    GET_DEVICE_FUNC_PTR(device, vkCreateSwapchainKHR);
//    GET_DEVICE_FUNC_PTR(device, vkDestroySwapchainKHR);
//    GET_DEVICE_FUNC_PTR(device, vkGetSwapchainImagesKHR);
}
