#include "VulkanCommandPool.h"
#include <assert.h>

VulkanCommandPool::VulkanCommandPool(VulkanDevice *vulkanDevice)
    : m_vulkanDevice(vulkanDevice), m_created(false)
{
    assert(m_vulkanDevice != nullptr);
}

VkResult VulkanCommandPool::create(uint32_t queueFamilyIndex,
                                   VkCommandPoolCreateFlags flags,
                                   const void *pNext)
{
    VkResult ret;
    VkCommandPoolCreateInfo cmdPoolInfo;

    if (m_created) {
        destroy();
    }
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.pNext = pNext;
    cmdPoolInfo.flags = flags;
    cmdPoolInfo.queueFamilyIndex = queueFamilyIndex;

    ret = vkCreateCommandPool(m_vulkanDevice->getVkDevice(), &cmdPoolInfo,
                              nullptr, &m_vkCommandPool);
    if (ret == VK_SUCCESS)
        m_created = true;
    return ret;
}

VkResult VulkanCommandPool::reset(VkCommandPoolResetFlags flags)
{
    VkResult ret;

    if (!m_created) {
        ret = VK_NOT_READY;
    } else {
        ret = vkResetCommandPool(m_vulkanDevice->getVkDevice(), m_vkCommandPool, flags);
    }
    return ret;
}

void VulkanCommandPool::destroy()
{
    if (m_created) {
        vkDestroyCommandPool(m_vulkanDevice->getVkDevice(),
                             m_vkCommandPool, nullptr);
    }
}

bool VulkanCommandPool::isCreated() const
{
    return m_created;
}

VkCommandPool VulkanCommandPool::getVkCommandPool() const
{
    return m_vkCommandPool;
}

const VulkanDevice *VulkanCommandPool::getVulkanDevice() const
{
    return m_vulkanDevice;
}

VkResult VulkanCommandPool::allocCommandBuffers(VkCommandBuffer *cmdBuf,
                                                uint32_t count,
                                                VkCommandBufferLevel level,
                                                void *pNext)
{
    VkResult ret;

    if (!m_created) {
        return VK_NOT_READY;
    }

    VkCommandBufferAllocateInfo cmdBufInfo;
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufInfo.pNext = pNext;
    cmdBufInfo.commandPool = m_vkCommandPool;
    cmdBufInfo.level = level;
    cmdBufInfo.commandBufferCount = count;

    ret = vkAllocateCommandBuffers(m_vulkanDevice->getVkDevice(), &cmdBufInfo, cmdBuf);
    return ret;
}






















