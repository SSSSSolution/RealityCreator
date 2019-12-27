#ifndef VULKANDEVICE_H
#define VULKANDEVICE_H

#include <vulkan/vulkan.h>

/* 逻辑设备(gpu)类
 */
class VulkanDevice
{
public:
    VulkanDevice(VkDevice device);
    VkDevice getVkDevice() const;

private:
    VkDevice m_vkDevice;
};

#endif // VULKANDEVICE_H
