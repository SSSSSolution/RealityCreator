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

    void initViewports(VkCommandBuffer *cmd);
    void initScissors(VkCommandBuffer *cmd);


private:
    void recordCommandBuffer(int currentImage, VkCommandBuffer *cmdDraw);

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
};

#endif // VULKANDRAWABLE_H
