#ifndef VULKANDEVICE_H
#define VULKANDEVICE_H

#include <vulkan/vulkan.h>

class VulkanPhysicalDevice;
/* 逻辑设备(gpu)类
 */
class VulkanDevice
{
public:
    VulkanDevice(VkDevice device, VulkanPhysicalDevice *physicalDevice);
    VkDevice getVkDevice() const;
    VkPhysicalDevice getPhysicalDevice() const;

    bool memoryTypeFromProperties(uint32_t typeBits, VkFlags requirementsMask,
                                  uint32_t *typeIndex);
    void getDeviceQueue();

public:
    VkQueue queue;
    uint32_t graphicsQueueWithPresentIndex;

private:
    VkDevice m_vkDevice;
    VulkanPhysicalDevice *m_physicalDevice;


};

#endif // VULKANDEVICE_H
