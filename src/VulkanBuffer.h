#ifndef VULKANBUFFER_H
#define VULKANBUFFER_H

#include <vulkan/vulkan.h>

class VulkanBuffer
{
public:
    VulkanBuffer();

private:
    VkBuffer m_vkBuffer;
};

#endif // VULKANBUFFER_H
