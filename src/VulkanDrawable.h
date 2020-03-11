#ifndef VULKANDRAWABLE_H
#define VULKANDRAWABLE_H
#include "headers.h"
class VulkanRenderer;

class VulkanDrawable
{
public:
    VulkanDrawable(VulkanRenderer *parent);
    ~VulkanDrawable();

    void createVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture);
    void prepare();
    void render();

    void setPipeline(VkPipeline *vkPipeline) { pipeline = vkPipeline; }


private:
    void recordCommandBuffer(int currentImage, VkCommandBuffer *cmdDraw);
    void initViewports(VkCommandBuffer *cmd);
    void initScissors(VkCommandBuffer *cmd);

public:
    struct {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo bufferInfo;
    } VertexBuffer;

    VkVertexInputBindingDescription viIpBind;
    VkVertexInputAttributeDescription viIpAttrb[2];

private:
    VulkanRenderer *vulkanRenderer;
    std::vector<VkCommandBuffer> vecCmdDraw;
    VkPipeline *pipeline;

    VkViewport viewport;
    VkRect2D scissor;

    VkFence vkFence;
    VkSemaphore presentCompleteSemaphore;
    VkSemaphore drawingCompleteSemaphore;
};

#endif // VULKANDRAWABLE_H
