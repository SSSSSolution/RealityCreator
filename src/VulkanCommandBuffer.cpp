#include "VulkanCommandBuffer.h"

VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandPool *vulkanCommandPool)
    : m_vulkanCommandPool(vulkanCommandPool), m_allocated(false)
{

}

VkResult VulkanCommandBuffer::alloc(VkCommandBufferLevel level,
                                    void *pNext)
{
    VkResult ret;

    if (m_allocated) {
        // todo
    }
    if (!m_vulkanCommandPool->isCreated()) {
        return VK_NOT_READY;
    }

    VkCommandBufferAllocateInfo cmdBufferAllocateInfo;
    cmdBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufferAllocateInfo.pNext = pNext;
    cmdBufferAllocateInfo.commandPool = m_vulkanCommandPool->getVkCommandPool();
    cmdBufferAllocateInfo.level = level;
    cmdBufferAllocateInfo.commandBufferCount = 1;

    ret = vkAllocateCommandBuffers(m_vulkanCommandPool->getVulkanDevice()->getVkDevice(),
                                   &cmdBufferAllocateInfo, &m_vkCommandBuffer);
    if (ret == VK_SUCCESS)
        m_allocated = true;
    return ret;
}

void VulkanCommandBuffer::free()
{
//    if (m_allocated) {
//        vkFreeCommandBuffers(m_vulkanCommandPool->getVulkanDevice()->getVkDevice(),
//                             m_vulkanCommandPool->getVkCommandPool(),
//                             );
//    }
}





















