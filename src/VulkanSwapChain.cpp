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
    managePresentMode();

    // Create the swap chain images
    createSwapChainColorImages();

    //
    createColorImageView(cmd);
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
        format = VK_FORMAT_B8G8R8A8_UNORM;
    } else {
        format = surfaceFormatList.at(0).format;
    }
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

    // Determine the number of VkImage's to use in the swap chain.
    desiredNumberOfSwapChainImages = surfaceCapabilities.minImageCount + 1;
    if ((surfaceCapabilities.maxImageCount > 0) && (desiredNumberOfSwapChainImages > surfaceCapabilities.maxImageCount)) {
        desiredNumberOfSwapChainImages = surfaceCapabilities.maxImageCount;
    }

    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        preTransform = surfaceCapabilities.currentTransform;
    }
}

void VulkanSwapChain::createSwapChainColorImages()
{
    VkResult ret;

    VkSwapchainCreateInfoKHR swapChainInfo = {};
    swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainInfo.pNext = nullptr;
    swapChainInfo.surface = surface;
    swapChainInfo.minImageCount = desiredNumberOfSwapChainImages;
    swapChainInfo.imageFormat = format;
    swapChainInfo.imageExtent.width = swapChainExtent.width;
    swapChainInfo.imageExtent.height = swapChainExtent.height;
    swapChainInfo.preTransform = preTransform;
    swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainInfo.imageArrayLayers = 1;
    swapChainInfo.presentMode = swapchainPresentMode;
    swapChainInfo.oldSwapchain = VK_NULL_HANDLE;
    swapChainInfo.clipped = true;
    swapChainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainInfo.queueFamilyIndexCount = 0;
    swapChainInfo.pQueueFamilyIndices = nullptr;

    ret = vkCreateSwapchainKHR(VulkanApplication::getInstance()->vulkanDevice.vkDevice, &swapChainInfo, nullptr, &swapChain);
    assert(ret == VK_SUCCESS);

    ret = vkGetSwapchainImagesKHR(VulkanApplication::getInstance()->vulkanDevice.vkDevice, swapChain, &swapChainImagesCount, nullptr);
    assert(ret == VK_SUCCESS);

    swapChainImages.clear();
    swapChainImages.resize(swapChainImagesCount);
    assert(swapChainImages.size() >= 1);

    ret = vkGetSwapchainImagesKHR(VulkanApplication::getInstance()->vulkanDevice.vkDevice, swapChain, &swapChainImagesCount, swapChainImages.data());
    assert(ret == VK_SUCCESS);
}

void VulkanSwapChain::createColorImageView(const VkCommandBuffer &cmd)
{
    VkResult ret;
    colorBufferList.clear();

    for (uint32_t i = 0; i < swapChainImagesCount; i++) {
        SwapChainBuffer sc_buffer;

        VkImageViewCreateInfo imgViewInfo = {};
        imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imgViewInfo.pNext = nullptr;
        imgViewInfo.format = format;
        imgViewInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY };
        imgViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgViewInfo.subresourceRange.baseMipLevel = 0;
        imgViewInfo.subresourceRange.levelCount = 1;
        imgViewInfo.subresourceRange.baseArrayLayer = 0;
        imgViewInfo.subresourceRange.layerCount = 1;
        imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imgViewInfo.flags = 0;

        sc_buffer.image = swapChainImages[i];

        imgViewInfo.image = sc_buffer.image;

        ret = vkCreateImageView(VulkanApplication::getInstance()->vulkanDevice.vkDevice, &imgViewInfo, nullptr, &sc_buffer.view);
        colorBufferList.push_back(sc_buffer);
        assert(ret == VK_SUCCESS);
    }
    currentColorBuffer = 0;
}

































