#include "VulkanSwapChain.h"
#include <assert.h>
#include <QDebug>

#define INSTANCE_FUNC_PTR(instance, entrypoint) {											\
    fp##entrypoint = (PFN_vk##entrypoint) vkGetInstanceProcAddr(instance, "vk"#entrypoint); \
    if (fp##entrypoint == nullptr) {                                                        \
        qDebug() << "Unable to locate the vkGetDeviceProcAddr: vk"#entrypoint;				\
        exit(-1);																			\
    }														 								\
}

#define DEVICE_FUNC_PTR(dev, entrypoint) {                                                  \
    fp##entrypoint = (PFN_vk##entrypoint) vkGetDeviceProcAddr(dev, "vk"#entrypoint);        \
if (fp##entrypoint == nullptr) {                                                            \
        qDebug() << "Unable to locate the vkGetDeviceProcAddr: vk"#entrypoint;              \
        exit(-1);                                                                           \
    }                                                                                       \
}

VulkanSwapChain::VulkanSwapChain(VulkanInstance *inst, VulkanDevice *device, VkSurfaceKHR surface)
    : m_instance(inst), m_device(device)
{
    assert(m_instance != nullptr);
    assert(m_device != nullptr);
    scPublicVars.surface = surface;
}

void VulkanSwapChain::intializeSwapChain()
{
    // Querying swapchain extensions
    createSwapChainExtensions();
}

void VulkanSwapChain::createSwapChainExtensions()
{
    VkInstance instance = m_instance->getVkInstance();
    VkDevice device = m_device->getVkDevice();

    INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceSupportKHR);
    INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
    INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceFormatsKHR);
    INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfacePresentModesKHR);
    INSTANCE_FUNC_PTR(instance, DestroySurfaceKHR);

    DEVICE_FUNC_PTR(device, CreateSwapchainKHR);
    DEVICE_FUNC_PTR(device, DestroySwapchainKHR);
    DEVICE_FUNC_PTR(device, GetSwapchainImagesKHR);
    DEVICE_FUNC_PTR(device, AcquireNextImageKHR);
    DEVICE_FUNC_PTR(device, QueuePresentKHR);
}

void VulkanSwapChain::createSwapChain(const VkCommandBuffer& cmd)
{
    getSurfaceCapabilitiesAndPresentMode();
}


void VulkanSwapChain::getSurfaceCapabilitiesAndPresentMode()
{
    VkResult result;
    VkPhysicalDevice gpu = m_device->getPhysicalDevice();

    result = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, scPublicVars.surface,
                                                       &scPrivateVars.surfCapabilities);
    assert(result == VK_SUCCESS);

    result = fpGetPhysicalDeviceSurfacePresentModesKHR(gpu, scPublicVars.surface,
                                                       &scPrivateVars.presentModeCount, nullptr);
    assert(result == VK_SUCCESS);
    scPrivateVars.presentModes.clear();
    scPrivateVars.presentModes.resize(scPrivateVars.presentModeCount);
    assert(scPrivateVars.presentModes.size() >= 1);

    result = fpGetPhysicalDeviceSurfacePresentModesKHR(gpu, scPublicVars.surface,
                                                       &scPrivateVars.presentModeCount,
                                                       scPrivateVars.presentModes.data());
    assert(result == VK_SUCCESS);

    if (scPrivateVars.surfCapabilities.currentExtent.width == (uint32_t)-1)
    {
        // if the surface width and height are not defined, set them equal to image size
//        scPrivateVars.swapChainExtent.width =
    } else {
        scPrivateVars.swapChainExtent = scPrivateVars.surfCapabilities.currentExtent;
    }
}

void VulkanSwapChain::managePresentMode()
{

}























