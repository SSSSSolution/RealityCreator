#ifndef VULKANDEVICE_H
#define VULKANDEVICE_H

#include <vulkan/vulkan.h>
#include <vector>
#include "VulkanLayerAndExtension.h"

class VulkanDevice {
public:
    VulkanDevice(VkPhysicalDevice *gpu);
    ~VulkanDevice();

    VkResult createDevice(std::vector<const char *>& layers,
                          std::vector<const char *>& extensions);
    void destroyDevice();

    bool memoryTypeFromProperties(uint32_t typeBits,
                                  VkFlags requirements_mask,
                                  uint32_t *typeIndex);

    // Get the avaiable queues exposed by the physical devices
    void getPhysicalDeviceQueueAndProperties();

    // Query physical device to retrive queue properties
    uint32_t getGraphicsQueueHandle();

    // Queue related member functions.
    void getDeviceQueue();

public:
    VkDevice device;
    VkPhysicalDevice *gpu;
    VkPhysicalDeviceProperties gpuProps;
    VkPhysicalDeviceMemoryProperties memoryProps;

    // Queue
    VkQueue queue;
    std::vector<VkQueueFamilyProperties> queueFamilyProps;
    uint32_t graphicsQueueIndex;
    uint32_t graphicsQueueWithPresentIndex;
    uint32_t queueFamilyCount;

    // Layer and extensions
    VulkanLayerAndExtension layerExtension;
};

#endif // VULKANDEVICE_H
