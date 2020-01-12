#include "VulkanSwapChain.h"
#include "VulkanRenderer.h"
#include "VulkanApplication.h"
#include <assert.h>
#include <iostream>

#define INSTANCE_FUNC_PTR(instance, entrypoint) {											\
    fp##entrypoint = (PFN_vk##entrypoint) vkGetInstanceProcAddr(instance, "vk"#entrypoint); \
    if (fp##entrypoint == nullptr) {                                                        \
        std::cout << "Unable to locate the vkGetDeviceProcAddr: vk"#entrypoint;				\
        exit(-1);																			\
    }														 								\
}

#define DEVICE_FUNC_PTR(dev, entrypoint) {                                                  \
    fp##entrypoint = (PFN_vk##entrypoint) vkGetDeviceProcAddr(dev, "vk"#entrypoint);        \
if (fp##entrypoint == nullptr) {                                                            \
        std::cout << "Unable to locate the vkGetDeviceProcAddr: vk"#entrypoint;              \
        exit(-1);                                                                           \
    }                                                                                       \
}

VulkanSwapChain::VulkanSwapChain(VulkanRenderer *rendererObj)
    : rendererObj(rendererObj)
{
    appObj = VulkanApplication::getInstance();
}

VulkanSwapChain::~VulkanSwapChain()
{
    scPrivateVars.swapchainImages.clear();
    scPrivateVars.surfFormats.clear();
    scPrivateVars.presentModes.clear();
}

VkResult VulkanSwapChain::createSwapChainExtensions()
{
    // Dependency on createPresentationWindow()
    VkInstance &instance = appObj->instanceObj.instance;
    VkDevice &device = appObj->deviceObj->device;

    // Get Instance based swap chain extension function pointer
    INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceSupportKHR);
    INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
    INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceFormatsKHR);
    INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfacePresentModesKHR);
    INSTANCE_FUNC_PTR(instance, DestroySurfaceKHR);

    // Get Device based swap chain extensions function pointer
    DEVICE_FUNC_PTR(device, CreateSwapchainKHR);
    DEVICE_FUNC_PTR(device, DestroySwapchainKHR);
    DEVICE_FUNC_PTR(device, GetSwapchainImagesKHR);
    DEVICE_FUNC_PTR(device, AcquireNextImageKHR);
    DEVICE_FUNC_PTR(device, QueuePresentKHR);

    return VK_SUCCESS;
}

VkResult VulkanSwapChain::createSurface()
{
    VkResult result;
    // Depends on createPresentationWindow(), need an empty window

    VkInstance &instance = appObj->instanceObj.instance;

#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.hinstance = rendererObj->connection;
    createInfo.hwnd = rendererObj->window;

    result = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &scPublicVars.surface);
#else
#endif

    assert(result == VK_SUCCESS);
    return result;
}

uint32_t VulkanSwapChain::getGraphicsQueueWithPresentationSupport()
{
    VulkanDevice *device = appObj->deviceObj;
    uint32_t queueCount = device->queueFamilyCount;
    VkPhysicalDevice gpu = *device->gpu;
    std::vector<VkQueueFamilyProperties> &queueProps = device->queueFamilyProps;

    VkBool32 *supportsPresent = (VkBool32*)malloc(queueCount * sizeof(VkBool32));
    for (uint32_t i = 0; i < queueCount; i++)
    {
        fpGetPhysicalDeviceSurfaceSupportKHR(gpu, i, scPublicVars.surface, &supportsPresent[i]);
    }

    // Search for a graphics queue and a present queue in the array of queue
    // family, try to find one that supports both
    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
    uint32_t presentQueueNodeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < queueCount; i++)
    {
    }


}































































