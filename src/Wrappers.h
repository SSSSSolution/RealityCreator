#ifndef WRAPPESR_H
#define WRAPPESR_H

#include <vulkan/vulkan.h>
class CommandBufferMgr
{
public:
    static void allocCommandBuffer(const VkDevice *device,
                                   const VkCommandPool cmdPool,
                                   VkCommandBuffer *cmdBuf,
                                   const VkCommandBufferAllocateInfo *commandBufferInfo = nullptr);

    static void beginCommandBuffer(const VkCommandBuffer cmdBuf,
                                   VkCommandBufferBeginInfo *inCmdBufInfo = nullptr);


    static void endCommandBuffer(VkCommandBuffer cmdBuf);
    static void submitCommandBuffer(const VkQueue &queue,
                                    const VkCommandBuffer* cmdBufList,
                                    const VkSubmitInfo *submitInfo = nullptr,
                                    const VkFence &fence = VK_NULL_HANDLE);

};

void *readFile(const char *spvFileName, size_t *fileSize);

#endif // WRAPPESR_H
