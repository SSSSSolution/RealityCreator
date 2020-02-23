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


public:
    VkPhysicalDevice vkPhysicalDevice;
    VkDevice vkDevice;

    PhysicalDeviceProperties physicalDeviceProperties;

    // Queue
    std::vector<VkQueueFamilyProperties> vkQueueFamilyPropertiesList;
    uint32_t graphicsQueueIndex;
    VkQueue deviceQueue;

private:
    void getPhysicalDeviceProperties(VkPhysicalDevice *gpu);
};

#endif // VULKANDEVICE_H
