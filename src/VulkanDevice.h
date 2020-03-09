#ifndef VULKANDEVICE_H
#define VULKANDEVICE_H

#include "headers.h"

struct PhysicalDeviceProperties
{
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceMemoryProperties memoryProperties;
};

class VulkanDevice
{
public:
    VulkanDevice();
    ~VulkanDevice();

    // 枚举gpu
    static VkResult enumeratePhysicalDevice(std::vector<VkPhysicalDevice> &gpuList);
    // 设置gpu
    void setVkPhysicalDevice(VkPhysicalDevice &gpu);
    VkResult createDevice(std::vector<const char *> layers, std::vector<const char *> extensions);

    bool memoryTypeFromProperties(uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);

    void getDeviceQueue();

public:
    VkPhysicalDevice vkPhysicalDevice;
    VkDevice vkDevice;

    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceMemoryProperties memoryProperties;

    // Queue
    VkQueue queue;
    std::vector<VkQueueFamilyProperties> vkQueueFamilyPropertiesList;
    uint32_t graphicsQueueIndex;
    uint32_t graphicsQueueWithPresentIndex;
    VkQueue deviceQueue;
    uint32_t queueFamilyCount;

private:
    void getPhysicalDeviceProperties(VkPhysicalDevice *gpu);
};

#endif // VULKANDEVICE_H
