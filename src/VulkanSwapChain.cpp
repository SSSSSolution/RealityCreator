#include "VulkanSwapChain.h"

#define GET_INSTANCE_FUNC_PTR(instance, funcname) {                                 \
    funcname = (PFN_##funcname)vkGetInstanceProcAddr(instance, #funcname);          \
    assert(funcname != nullptr);                                                    \
}

#define GET_DEVICE_FUNC_PTR(dev, funcname) {                                        \
    funcname = (PFN_##funcname)vkGetDeviceProcAddr(dev, #funcname);                 \
    assert(funcname != nullptr);                                                    \
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
    uint32_t index = getGraphicsQueueWithPresentationSupport();
    vulkanApplication->vulkanDevice.graphicsQueueWithPresentIndex = index;
    getSupportedFormats();
}

void VulkanSwapChain::createSwapChain(const VkCommandBuffer &cmd)
{
    /* This function retreive swapchain image and create those images- image view */

    // use extensions and get the surface capabilities, present mode
    getSurfaceCapabilitiesAndPresentMode();

    // Gather necessary information for present mode.

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
#elif __linux__
    VkXcbSurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.connection = vulkanRenderer->connection;
    createInfo.window = vulkanRenderer->window;

    ret = vkCreateXcbSurfaceKHR(instance, &createInfo, nullptr, &surface);
#endif
    assert(ret == VK_SUCCESS);
}

uint32_t VulkanSwapChain::getGraphicsQueueWithPresentationSupport()
{
    VulkanDevice *device = &vulkanApplication->vulkanDevice;
    uint32_t queueCount = device->queueFamilyCount;
    VkPhysicalDevice gpu = device->vkPhysicalDevice;
    std::vector<VkQueueFamilyProperties> &queueProps = device->vkQueueFamilyPropertiesList;

    // Iterate over each queue and get presentation status for each
    VkBool32 *supportPresent = (VkBool32 *)malloc(queueCount * sizeof(VkBool32));
    for (uint32_t i = 0; i < queueCount; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &supportPresent[i]);
    }

    // Search for a graphics queue and present queue in the array of queue families, try to
    // find one that supports both
    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
    uint32_t presentQueueNodeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < queueCount; i++) {
        if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            if (graphicsQueueNodeIndex == UINT32_MAX) {
                graphicsQueueNodeIndex = i;
            }
            if (supportPresent[i] == VK_TRUE) {
                graphicsQueueNodeIndex = i;
                presentQueueNodeIndex = i;
                break;
            }
        }
    }

    // If don't find a queue that supports both graphics and presetn, then
    // find a separate present queue.
    if (presentQueueNodeIndex == UINT32_MAX) {
        for (uint32_t i = 0; i < queueCount; ++i) {
            if (supportPresent[i] == VK_TRUE) {
                presentQueueNodeIndex = i;
                break;
            }
        }
    }

    free(supportPresent);

    // Generate error if could not find both a graphcis and a present queue.
    assert(graphicsQueueNodeIndex != UINT32_MAX && presentQueueNodeIndex != UINT32_MAX);

    return graphicsQueueNodeIndex;
}

void VulkanSwapChain::getSupportedFormats()
{
    VkPhysicalDevice gpu = vulkanApplication->vulkanDevice.vkPhysicalDevice;
    VkResult ret;

    // Get the list of VkFormats that are supported
    uint32_t formatCount;
    ret = vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, nullptr);
    assert(ret == VK_SUCCESS);

    surfaceFormatList.resize(formatCount);
    ret = vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, surfaceFormatList.data());
    assert(ret == VK_SUCCESS);

    // In case it's a VK_FORMAT_UNDEFINED, then surface has no preferred format. We use BGRA 32 bit format.
    if (formatCount == 1 && surfaceFormatList.at(0).format == VK_FORMAT_UNDEFINED)
    {
        surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
    } else {
        surfaceFormat.format = surfaceFormatList.at(0).format;
    }

    qDebug() << "surfaceFormat" << surfaceFormat.format;
}

void VulkanSwapChain::getSurfaceCapabilitiesAndPresentMode()
{
    VkResult ret;
    VkPhysicalDevice gpu = vulkanApplication->vulkanDevice.vkPhysicalDevice;

    ret = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &surfaceCapabilities);
    assert(ret == VK_SUCCESS);

    ret = vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, nullptr);
    assert(ret == VK_SUCCESS);

    presentModeList.clear();
    presentModeList.resize(presentModeCount);
    ret = vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, presentModeList.data());
    assert(ret == VK_SUCCESS);

    if (surfaceCapabilities.currentExtent.width == (uint32_t)-1) {
        swapChainExtent.width = vulkanRenderer->width;
        swapChainExtent.height = vulkanRenderer->height;
    } else {
        swapChainExtent = surfaceCapabilities.currentExtent;
    }

}

void VulkanSwapChain::managePresentMode()
{
    swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (size_t i = 0; i < presentModeCount; i++) {
        if (presentModeList[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
        if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) &&
            (presentModeList[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
        {
            swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }
}





































