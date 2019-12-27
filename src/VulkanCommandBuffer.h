#ifndef VULKANCOMMANDBUFFER_H
#define VULKANCOMMANDBUFFER_H

#include "vulkan/vulkan.h"
#include "VulkanCommandPool.h"

class VulkanCommandBuffer
{
    friend class VulkanCommandPool;

private:
    VulkanCommandBuffer(VulkanCommandPool *vulkanCommandPool);

    VkResult alloc(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                   void *pNext = nullptr);
    void free();

private:
    VulkanCommandPool *m_vulkanCommandPool;
    VkCommandBuffer m_vkCommandBuffer;

    bool m_allocated;
};

#endif // VULKANCOMMANDBUFFER_H
