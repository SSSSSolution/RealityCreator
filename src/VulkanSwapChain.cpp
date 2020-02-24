#include "VulkanSwapChain.h"

#define GET_INSTANCE_FUNC_PTR(instance, funcname) {                                  \
    funcname = (PFN_##funcname)vkGetInstanceProcAddr(instance, #funcname);       \
    assert(funcname != nullptr);                                                \
}

#define GET_DEVICE_FUNC_PTR(dev, funcname) {                        \
    funcname = (PFN_##funcname)vkGetDeviceProcAddr(dev, #funcname); \
    assert(funcname != nullptr);                                    \
}

VulkanSwapChain::VulkanSwapChain(VulkanRenderer *vulkanRenderer)
    : vulkanRenderer(vulkanRenderer)
{
    vulkanApplication = VulkanApplication::getInstance();
}

void VulkanSwapChain::initialize()
{
    querySwapChainExtensions();
    createSurface();
}

void VulkanSwapChain::querySwapChainExtensions()
{
    VkInstance instance = vulkanApplication->vulkanInstance.vkInstance;
    VkDevice device = vulkanApplication->vulkanDevice.vkDevice;

    GET_INSTANCE_FUNC_PTR(instance, vkGetPhysicalDeviceSurfaceSupportKHR);
    GET_INSTANCE_FUNC_PTR(instance, vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    GET_INSTANCE_FUNC_PTR(instance, vkGetPhysicalDeviceSurfaceFormatsKHR);
    GET_INSTANCE_FUNC_PTR(instance, vkGetPhysicalDeviceSurfacePresentModesKHR);
    GET_INSTANCE_FUNC_PTR(instance, vkDestroySurfaceKHR);

    GET_DEVICE_FUNC_PTR(device, vkCreateSwapchainKHR);
    GET_DEVICE_FUNC_PTR(device, vkDestroySwapchainKHR);
    GET_DEVICE_FUNC_PTR(device, vkGetSwapchainImagesKHR);
    GET_DEVICE_FUNC_PTR(device, vkQueuePresentKHR);
    GET_DEVICE_FUNC_PTR(device, vkAcquireNextImageKHR);
}

void VulkanSwapChain::createSurface()
{
    VkResult ret;
    VkInstance &instance = vulkanApplication->vulkanInstance.vkInstance;

#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.hinstance = vulkanRenderer->connection;
    createInfo.hwnd = vulkanRenderer->window;

    ret = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface);
    assert(ret == VK_SUCCESS);
#elif __linux__
#endif
}













































