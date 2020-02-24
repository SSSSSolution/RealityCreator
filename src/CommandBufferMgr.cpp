#include "CommandBufferMgr.h"

void CommandBufferMgr::allocCommandBuffer(const VkDevice *device, const VkCommandPool cmdPool,
                                          VkCommandBuffer *cmdBuf, const VkCommandBufferAllocateInfo *commandBufferInfo)
{
    VkResult ret;

    if (commandBufferInfo) {
        ret = vkAllocateCommandBuffers(*device, commandBufferInfo, cmdBuf);
        assert(ret == VK_SUCCESS);
        return;
    }

    VkCommandBufferAllocateInfo cmdInfo = {};
    cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdInfo.pNext = nullptr;
    cmdInfo.commandPool = cmdPool;
    cmdInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdInfo.commandBufferCount = (uint32_t)sizeof(cmdBuf)/sizeof(VkCommandBuffer); // ?

    ret = vkAllocateCommandBuffers(*device, &cmdInfo, cmdBuf);
    assert(ret == VK_SUCCESS);
}

void CommandBufferMgr::beginCommandBuffer(VkCommandBuffer cmdBuf, VkCommandBufferBeginInfo *inCmdBufInfo)
{
    VkResult ret;

    if (inCmdBufInfo) {
        ret = vkBeginCommandBuffer(cmdBuf, inCmdBufInfo);
        assert(ret == VK_SUCCESS);
        return;
    }

    VkCommandBufferInheritanceInfo cmdBufInheritInfo = {};
    cmdBufInheritInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    cmdBufInheritInfo.pNext = nullptr;
    cmdBufInheritInfo.renderPass = VK_NULL_HANDLE;
    cmdBufInheritInfo.subpass = 0;
    cmdBufInheritInfo.framebuffer = VK_NULL_HANDLE;
    cmdBufInheritInfo.occlusionQueryEnable = VK_FALSE;
    cmdBufInheritInfo.queryFlags = 0;
    cmdBufInheritInfo.pipelineStatistics = 0;

    VkCommandBufferBeginInfo cmdBufBeginInfo = {};
    cmdBufBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufBeginInfo.pNext = nullptr;
    cmdBufBeginInfo.flags = 0;
    cmdBufBeginInfo.pInheritanceInfo = &cmdBufInheritInfo;

    ret = vkBeginCommandBuffer(cmdBuf, &cmdBufBeginInfo);
    assert(ret == VK_SUCCESS);
}

void CommandBufferMgr::endCommandBuffer(VkCommandBuffer cmdBuf)
{
    VkResult ret;

    ret = vkEndCommandBuffer(cmdBuf);
    assert(ret == VK_SUCCESS);
}

void CommandBufferMgr::submitCommandBuffer(const VkQueue &queue, const VkCommandBuffer *cmdBufList, const VkSubmitInfo *inSubmitInfo, const VkFence &fence)
{
    VkResult ret;

    if (inSubmitInfo) {
        ret = vkQueueSubmit(queue, 1, inSubmitInfo, fence);
        assert(ret == VK_SUCCESS);

        ret = vkQueueWaitIdle(queue);
        assert(ret == VK_SUCCESS);
        return;
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = (uint32_t)sizeof(cmdBufList)/sizeof(VkCommandBuffer);
    submitInfo.pCommandBuffers = cmdBufList;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    ret = vkQueueSubmit(queue, 1, &submitInfo, fence);
    assert(ret == VK_SUCCESS);

    ret = vkQueueWaitIdle(queue);
    assert(ret == VK_SUCCESS);
}


























