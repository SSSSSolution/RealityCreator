#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
#include <assert.h>

VulkanDevice::VulkanDevice(VkPhysicalDevice *physicalDevice)
{
    gpu = physicalDevice;
}

VulkanDevice::~VulkanDevice()
{

}

VkResult VulkanDevice::createDevice(std::vector<const char *> &layers,
                                    std::vector<const char *> &extensions)
{
    layerExtension.appRequestedLayerNames = layers;
    layerExtension .appRequestedExtensionNames = extensions;

    // Create Device with avaiable queue information

    VkResult result;
    float queuePriorities[1] = { 0.0f };
    VkDeviceQueueCreateInfo queueInfo = {};
    queueInfo.queueFamilyIndex = graphicsQueueIndex;
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = nullptr;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queuePriorities;

    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = nullptr;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = nullptr;
    deviceInfo.enabledExtensionCount = (uint32_t)extensions.size();
    deviceInfo.ppEnabledExtensionNames = extensions.size() ? extensions.data() : nullptr;
    deviceInfo.pEnabledFeatures = nullptr;

    result = vkCreateDevice(*gpu, &deviceInfo, nullptr, &device);
    assert(result == VK_SUCCESS);

    return result;
}

bool VulkanDevice::memoryTypeFromProperties(uint32_t typeBits,
                                            VkFlags requirements_mask,
                                            uint32_t *typeIndex)
{
    for (uint32_t i = 0; i < 32; i++)
    {
        if ((typeBits & 1) == 1) {
            if ((memoryProps.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }

    return false;
}

void VulkanDevice::getPhysicalDeviceQueueAndProperties()
{
    vkGetPhysicalDeviceQueueFamilyProperties(*gpu, &queueFamilyCount, nullptr);

    // Allocate space to accomodate Queue properties
    queueFamilyProps.resize(queueFamilyCount);

    // Get queue family properties
    vkGetPhysicalDeviceQueueFamilyProperties(*gpu, &queueFamilyCount, queueFamilyProps.data());
}

uint32_t VulkanDevice::getGraphicsQueueHandle()
{
    // 1. Get the number of Queues supported by the Physical device
    // 2. Get the properties each Queue type or Queue Family

    bool found = false;
    // 1. Iterate number of Queues supported by the Physical device
    for (unsigned int i = 0; i < queueFamilyCount; i++)
    {
        // 2. Get the Graphics Queue type
        //      The could be 4 Queue type or Queue families supported by physical device -
        //      Graphics Queue      - VK_QUEUE_GRAPHICS_BIT
        //      Compute Queue       - VK_QUEUE_COMPUTE_BIT
        //      DMA/Transfer Queue  - VK_QUEUE_TRANSFER_BIT
        //      Sparse memory       - VK_QUEUE_SPARSE_BINDING_BIT
        if (queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            // 3. Get the handle/index ID of graphics queue family.
            found = true;
            graphicsQueueIndex = i;
            break;
        }
    }

    assert(found);

    return 0;
}

void VulkanDevice::destroyDevice()
{
    vkDestroyDevice(device, nullptr);
}

void VulkanDevice::getDeviceQueue()
{
    vkGetDeviceQueue(device, graphicsQueueWithPresentIndex, 0, &queue);
}































































