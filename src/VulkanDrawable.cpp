#include "VulkanDrawable.h"
#include "VulkanApplication.h"
#include "VulkanDevice.h"
#include "VulkanRenderer.h"
#include "VulkanSwapChain.h"
#include "CommandBufferMgr.h"

VulkanDrawable::VulkanDrawable(VulkanRenderer *parent)
{
    vulkanRenderer = parent;

    VkSemaphoreCreateInfo presentCompleteSemaphoreCreateInfo;
    presentCompleteSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    presentCompleteSemaphoreCreateInfo.pNext = nullptr;
    presentCompleteSemaphoreCreateInfo.flags = 0;

    VkSemaphoreCreateInfo drawingSemaphoreCreateInfo;
    drawingSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    drawingSemaphoreCreateInfo.pNext = nullptr;
    drawingSemaphoreCreateInfo.flags = 0;

    VulkanDevice *vulkanDevice = &VulkanApplication::getInstance()->vulkanDevice;

    vkCreateSemaphore(vulkanDevice->vkDevice, &presentCompleteSemaphoreCreateInfo, nullptr, &presentCompleteSemaphore);
    vkCreateSemaphore(vulkanDevice->vkDevice, &drawingSemaphoreCreateInfo, nullptr, &drawingCompleteSemaphore);
}

VulkanDrawable::~VulkanDrawable()
{

}

void VulkanDrawable::createVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture)
{
    VulkanApplication *vulkanApplication = VulkanApplication::getInstance();
    VulkanDevice *vulkanDevice = &vulkanApplication->vulkanDevice;

    VkResult ret;
    bool pass;

    // Create the Buffer resource metadata information
    VkBufferCreateInfo bufInfo = {};
    bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufInfo.pNext = nullptr;
    bufInfo.flags = 0;
    bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufInfo.size = dataSize;
    bufInfo.queueFamilyIndexCount = 0;
    bufInfo.pQueueFamilyIndices = nullptr;
    bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // Create the Buffer resource
    ret = vkCreateBuffer(vulkanDevice->vkDevice, &bufInfo, nullptr, &VertexBuffer.buf);
    assert(ret == VK_SUCCESS);

    // Get the Buffer  resource requirements
    VkMemoryRequirements memRqrmnt;
    vkGetBufferMemoryRequirements(vulkanDevice->vkDevice, VertexBuffer.buf, &memRqrmnt);

    // Create memory allocation metadata information
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.memoryTypeIndex = 0;
    allocInfo.allocationSize = memRqrmnt.size;

    // Get the compatible type of memory
    pass = vulkanDevice->memoryTypeFromProperties(memRqrmnt.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &allocInfo.memoryTypeIndex);
    assert(pass);

    // Allocate the physical device memory region to the host
    ret = vkAllocateMemory(vulkanDevice->vkDevice, &allocInfo, nullptr, &(VertexBuffer.mem));
    assert(ret == VK_SUCCESS);
    VertexBuffer.bufferInfo.range = memRqrmnt.size;
    VertexBuffer.bufferInfo.offset = 0;

    // Map the physical device memory region to the host
    uint8_t *pData;
    ret = vkMapMemory(vulkanDevice->vkDevice, VertexBuffer.mem, 0, memRqrmnt.size, 0, (void **)&pData);
    assert(ret == VK_SUCCESS);

    // Copy the data in the mapped memory
    memcpy(pData, vertexData, dataSize);

    // Unmap the device memory
    vkUnmapMemory(vulkanDevice->vkDevice, VertexBuffer.mem);

    // Bind the allocated buffer resource to the device memory
    ret = vkBindBufferMemory(vulkanDevice->vkDevice, VertexBuffer.buf, VertexBuffer.mem, 0);
    assert(ret == VK_SUCCESS);

    viIpBind.binding = 0;
    viIpBind.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    viIpBind.stride = dataStride;

    viIpAttrb[0].binding = 0;
    viIpAttrb[0].location = 0;
    viIpAttrb[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    viIpAttrb[0].offset = 0;
    viIpAttrb[1].binding = 0;
    viIpAttrb[1].location = 1;
    viIpAttrb[1].format = useTexture ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
    viIpAttrb[1].offset = 16;
}


void VulkanDrawable::prepare()
{
    VulkanDevice *vulkanDevice = vulkanRenderer->vulkanDevice;
    vecCmdDraw.resize(vulkanRenderer->vulkanSwapChain->colorBufferList.size());

    for (uint32_t i = 0; i < vulkanRenderer->vulkanSwapChain->colorBufferList.size(); i++) {
        CommandBufferMgr::allocCommandBuffer(&vulkanDevice->vkDevice, vulkanRenderer->cmdPool, &vecCmdDraw[i]);
        CommandBufferMgr::beginCommandBuffer(vecCmdDraw[i]);

        // Create the render pass instance
        recordCommandBuffer(i, &vecCmdDraw[i]);

        CommandBufferMgr::endCommandBuffer(vecCmdDraw[i]);
    }
}

void VulkanDrawable::recordCommandBuffer(int currentImage, VkCommandBuffer *cmdDraw)
{
    VulkanDevice *vulkanDevice = vulkanRenderer->vulkanDevice;

    VkClearValue clearValues[2];
    clearValues[0].color.float32[0] = 0.0f;
    clearValues[0].color.float32[1] = 0.0f;
    clearValues[0].color.float32[2] = 0.0f;
    clearValues[0].color.float32[3] = 0.0f;

    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;

    VkRenderPassBeginInfo renderPassBegin = {};
    renderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBegin.pNext = nullptr;
    renderPassBegin.renderPass = vulkanRenderer->renderPass;
    renderPassBegin.framebuffer = vulkanRenderer->framebuffers[currentImage];
    renderPassBegin.renderArea.offset.x = 0;
    renderPassBegin.renderArea.offset.y = 0;
    renderPassBegin.renderArea.extent.width = vulkanRenderer->width;
    renderPassBegin.renderArea.extent.height = vulkanRenderer->height;
    renderPassBegin.clearValueCount = 2;
    renderPassBegin.pClearValues = clearValues;

    // Start recording the render pass instance
    vkCmdBeginRenderPass(*cmdDraw, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);

    // Bound the command buffer with the graphics pipeline
    vkCmdBindPipeline(*cmdDraw, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);

    // Bound the command buffer with the graphics pipeline
    const VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(*cmdDraw, 0, 1, &VertexBuffer.buf, offsets);

    // Define the dynamic viweport here
    initViewports(cmdDraw);

    initScissors(cmdDraw);

    vkCmdDraw(*cmdDraw, 3, 1, 0, 0);

    vkCmdEndRenderPass(*cmdDraw);
}

void VulkanDrawable::render()
{
    VkResult ret;

    VulkanDevice *vulkanDevice = vulkanRenderer->vulkanDevice;
    VulkanSwapChain *vulkanSwapChain = vulkanRenderer->vulkanSwapChain;

    uint32_t &currentColorImage = vulkanSwapChain->currentColorBuffer;
    qDebug() << currentColorImage;
    VkSwapchainKHR &swapChain = vulkanSwapChain->swapChain;

    VkFence nullFence = VK_NULL_HANDLE;

    ret = vulkanSwapChain->vkAcquireNextImageKHR(vulkanDevice->vkDevice, swapChain, UINT64_MAX,
                                                 presentCompleteSemaphore, VK_NULL_HANDLE, &currentColorImage);
    qDebug() << ret;
    VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &presentCompleteSemaphore;
    submitInfo.pWaitDstStageMask = &submitPipelineStages;
    submitInfo.commandBufferCount = (uint32_t)sizeof(&vecCmdDraw[currentColorImage])/sizeof(VkCommandBuffer);
    submitInfo.pCommandBuffers = &vecCmdDraw[currentColorImage];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &drawingCompleteSemaphore;

    // Queue the command buffer for execution
    CommandBufferMgr::submitCommandBuffer(vulkanDevice->queue, &vecCmdDraw[currentColorImage], &submitInfo);

    //present the image in the window
    VkPresentInfoKHR present = {};
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = nullptr;
    present.swapchainCount = 1;
    present.pSwapchains = &swapChain;
    present.pImageIndices = &currentColorImage;
    present.pWaitSemaphores                                                                                                                                                    = &drawingCompleteSemaphore;
    present.waitSemaphoreCount = 1;
    present.pResults = nullptr;

    // Queue the image for presentation
    ret = vulkanSwapChain->vkQueuePresentKHR(vulkanDevice->queue, &present);
    assert(ret == VK_SUCCESS);

}

#define NUMBER_OF_VIEWPORTS 1
#define NUMBER_OF_SCISSORS NUMBER_OF_VIEWPORTS

void VulkanDrawable::initViewports(VkCommandBuffer *cmd)
{
    viewport.height = (float)vulkanRenderer->height;
    viewport.width = (float)vulkanRenderer->width;
    viewport.minDepth = (float)0.0f;
    viewport.maxDepth = (float)1.0f;
    viewport.x = 0;
    viewport.y = 0;
    vkCmdSetViewport(*cmd, 0, NUMBER_OF_VIEWPORTS, &viewport);
}

void VulkanDrawable::initScissors(VkCommandBuffer *cmd)
{
    scissor.extent.width = vulkanRenderer->width;
    scissor.extent.height = vulkanRenderer->height;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    vkCmdSetScissor(*cmd, 0, NUMBER_OF_SCISSORS, &scissor);
}













































