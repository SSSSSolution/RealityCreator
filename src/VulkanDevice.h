#ifndef VULKANDEVICE_H
#define VULKANDEVICE_H

#include <vulkan/vulkan.h>

/* 逻辑设备(gpu)类
 */
class VulkanDevice
{
public:
    VulkanDevice(VkDevice device);

private:
    VkDevice m_device;
};

#endif // VULKANDEVICE_H
