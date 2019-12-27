#ifndef VULKANCOMMANDPOOL_H
#define VULKANCOMMANDPOOL_H

#include <vulkan/vulkan.h>
#include "VulkanDevice.h"

class VulkanCommandPool
{
public:
    VulkanCommandPool(VulkanDevice *vulkanDevice);
    VkResult create(uint32_t queueFamilyIndex,
                    VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                    const void *pNext = nullptr );
    VkResult reset(VkCommandPoolResetFlags flags);
    void destroy();

    bool isCreated() const;
    VkCommandPool getVkCommandPool() const;
    const VulkanDevice *getVulkanDevice() const;

    VkResult allocCommandBuffers(VkCommandBuffer *cmdBuf,
                                 uint32_t count,
                                 VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                 void *pNext = nullptr);
private:
    VulkanDevice *m_vulkanDevice;
    VkCommandPool m_vkCommandPool;

    bool m_created;
};

#endif // VULKANCOMMANDPOOL_H
