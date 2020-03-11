#include "VulkanDevice.h"
#include "VulkanApplication.h"

VulkanDevice::VulkanDevice()
{

}

VulkanDevice::~VulkanDevice()
{

}

VkResult VulkanDevice::createDevice(std::vector<const char *> layers, std::vector<const char *> extensions)
{
    VkResult ret;
    float queuePriorities[1] = { 0.0f };

    VkDeviceQueueCreateInfo queueInfo = {};
    queueInfo.queueFamilyIndex =graphicsQueueIndex;
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = nullptr;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queuePriorities;

    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = nullptr;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    deviceInfo.ppEnabledLayerNames = layers.data();
    deviceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    deviceInfo.ppEnabledExtensionNames = extensions.data();
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.depthClamp = VK_TRUE;
    deviceInfo.pEnabledFeatures = &deviceFeatures;

    ret = vkCreateDevice(vkPhysicalDevice, &deviceInfo, nullptr, &vkDevice);
    assert(ret == VK_SUCCESS);

    vkGetDeviceQueue(vkDevice, graphicsQueueIndex, 0, &deviceQueue);
    getPhysicalDeviceProperties(&vkPhysicalDevice);
    return ret;
}

VkResult VulkanDevice::enumeratePhysicalDevice(std::vector<VkPhysicalDevice> &gpuList)
{
    uint32_t gpuCount;
    VkResult ret;

    ret = vkEnumeratePhysicalDevices(VulkanApplication::getInstance()->vulkanInstance.vkInstance, &gpuCount, nullptr);
    assert(ret == VK_SUCCESS);
    qDebug() << "gpu count: " << gpuCount;

    gpuList.resize(gpuCount);
    ret = vkEnumeratePhysicalDevices(VulkanApplication::getInstance()->vulkanInstance.vkInstance, &gpuCount, gpuList.data());
    assert(ret == VK_SUCCESS);

    return ret;
}

void VulkanDevice::setVkPhysicalDevice(VkPhysicalDevice &gpu)
{
    vkPhysicalDevice = gpu;

    // Get queue info
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
    qDebug() << "queueFamilyCount: " << queueFamilyCount;

    vkQueueFamilyPropertiesList.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, vkQueueFamilyPropertiesList.data());
    for (uint32_t i = 0; i < vkQueueFamilyPropertiesList.size(); i++)
    {
        qDebug() << "queue flags: " << vkQueueFamilyPropertiesList.at(i).queueFlags;
        qDebug() << "queue count: " << vkQueueFamilyPropertiesList.at(i).queueCount;
        if (vkQueueFamilyPropertiesList.at(i).queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            qDebug() << "graphics queue";
            graphicsQueueIndex = i;
        }
        else if (vkQueueFamilyPropertiesList.at(i).queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            qDebug() << "compute queue";
        } else if (vkQueueFamilyPropertiesList.at(i).queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            qDebug() << "transfer queue";
        }
    }
}

void VulkanDevice::getPhysicalDeviceProperties(VkPhysicalDevice *gpu)
{
    vkGetPhysicalDeviceProperties(*gpu, &physicalDeviceProperties);
    vkGetPhysicalDeviceMemoryProperties(*gpu, &memoryProperties);
}

void VulkanDevice::getDeviceQueue()
{
    // Parminder: this depends on inatlling the swapchain to
    // get the grpaphhics
    vkGetDeviceQueue(vkDevice, graphicsQueueIndex, 0, &queue);
}

bool VulkanDevice::memoryTypeFromProperties(uint32_t typeBits, VkFlags requirementsMask, uint32_t *typeIndex)
{
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < 32; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}











