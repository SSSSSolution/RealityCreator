#ifndef VULKANDRAWABLE_H
#define VULKANDRAWABLE_H
#include "headers.h"
#include "VulkanDescriptor.h"

class VulkanRenderer;

class VulkanDrawable : VulkanDescriptor
{
public:
    VulkanDrawable(VulkanRenderer *parent);
    ~VulkanDrawable();

    void createVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture);
    void prepare();
    void render();

    void setPipeline(VkPipeline *vkPipeline) { pipeline = vkPipeline; }

    virtual void createDescriptorSetLayout(bool useTexture) override;
    virtual void createPipelineLayout() override;
    virtual void createDescriptorPool(bool useTexture) override;
    virtual void createDescriptorResource() override;
    virtual void createDescriptorSet(bool useTexture) override;

private:
    void recordCommandBuffer(int currentImage, VkCommandBuffer *cmdDraw);
    void initViewports(VkCommandBuffer *cmd);
    void initScissors(VkCommandBuffer *cmd);

    void createUniformBuffer();

public:
    struct {
        VkBuffer buffer;
        VkDeviceMemory memory;
        VkDescriptorBufferInfo bufferInfo;
        VkMemoryRequirements memRqrmnt;
        std::vector<VkMappedMemoryRange> mappedRange;
        uint8_t* pData;
    } UniformData;

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

    bool prepared;

    glm::mat4 Projection;
    glm::mat4 View;
    glm::mat4 Model;
    glm::mat4 MVP;
};

#endif // VULKANDRAWABLE_H
