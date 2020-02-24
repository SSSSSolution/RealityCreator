#ifndef COMMANDBUFFERMGR_H
#define COMMANDBUFFERMGR_H

#include "headers.h"

class CommandBufferMgr
{
public:
    static void allocCommandBuffer(const VkDevice *device, const VkCommandPool cmdPool, VkCommandBuffer *cmdBuf, const VkCommandBufferAllocateInfo *commandBufferInfo = nullptr);
    static void beginCommandBuffer(VkCommandBuffer cmdBuf, VkCommandBufferBeginInfo *inCmdBufInfo = nullptr);
    static void endCommandBuffer(VkCommandBuffer cmdBuf);
    static void submitCommandBuffer(const VkQueue& queue, const VkCommandBuffer *cmdBufList, const VkSubmitInfo *submitInfo = nullptr, const VkFence &fence = VK_NULL_HANDLE);
};

#endif // COMMANDBUFFERMGR_H
