#ifndef VULKANDEVICE_H
#define VULKANDEVICE_H

#include <vulkan/vulkan.h>

/* 逻辑设备(gpu)类
 */
class VulkanDevice
{
public:
    VulkanDevice(VkDevice device, VkPhysicalDevice physicalDevice);
    VkDevice getVkDevice() const;
    VkPhysicalDevice getPhysicalDevice() const;

private:
    VkDevice m_vkDevice;
    VkPhysicalDevice m_physical_device;
};

#endif // VULKANDEVICE_H
