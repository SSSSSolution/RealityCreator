#ifndef VULKANDRAWABLE_H
#define VULKANDRAWABLE_H

#include "vulkan/vulkan.h"
#include "VulkanDevice.h"
#include <vector>

class VulkanRenderer;

class VulkanDrawable
{
public:
    VulkanDrawable(VulkanRenderer *parent = nullptr);
    ~VulkanDrawable();

    void prepare();
    void render();
    void setPipeline(VkPipeline *vulkanPipeline) { pipeline = vulkanPipeline; }
    void initViewports(VkCommandBuffer *cmd);
    void initScissors(VkCommandBuffer *cmd);

    void destroyCommandBuffer();

    void createVertexBuffer(const void *vertexData,
                            uint32_t dataSize,
                            uint32_t dataStride,
                            bool useTexture);
    void destroyVertexBuffer();

private:
//    void recordCommandBuffer(int currentImage, VkCommandBuffer *cmdDraw);

public:
    struct {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo bufferInfo;
    } vertexBuffer;

    VkVertexInputBindingDescription viIpBind;
    VkVertexInputAttributeDescription viIpAttrb[2];
    VulkanRenderer *rendererObj;

private:
    std::vector<VkCommandBuffer> vecCmdDraw;

    VkViewport viewport;
    VkRect2D scissor;
    VkSemaphore presentCompleteSemaphore;
    VkSemaphore drawingCompleteSemaphore;

    VkPipeline *pipeline;
};

#endif // VULKANDRAWABLE_H



















