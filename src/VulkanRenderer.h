#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <vulkan/vulkan.h>
#include "VulkanSwapChain.h"
class VulkanDevice;

#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

class VulkanRenderer
{
public:
    VulkanRenderer(VulkanDevice *device);
    ~VulkanRenderer() {}

    void initialize();
    bool render();

    VulkanDevice *getVulkanDevice();
    // Record render pass command buffer
    void createRenderPassCB(bool includeDepth);

    void setImageLayout(VkImage image,
                        VkImageAspectFlags aspectMask,
                        VkImageLayout oldImageLayout,
                        VkImageLayout newImageLayout,
                        VkAccessFlagBits srcAccessMask,
                        const VkCommandBuffer &cmdBuf);

    // Render pass createion
    void createRenderPass(bool includeDepth, bool clear = true);

    // Destroy the render pass object when no more required
    void destroyRenderPass();

    void createCommandPool();
    void buildSwapChainAndDepthImage();
    void createDepthImage();
    void createVertexBuffer();


    VulkanSwapChain *getSwapChain() {return swapchainObj; }
    VkCommandPool *getCommandPool() { return &m_cmdPool; }


    struct {
        VkFormat format;
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    } Depth;

    VkCommandBuffer cmdDepthImage;
    VkCommandPool cmdPool;
    VkCommandBuffer cmdVertexBuffer;

    // Render pass created object
    VkRenderPass renderPass;

    int width, height;

private:
    VulkanDevice *m_device;
    VkCommandPool m_cmdPool;

    VulkanSwapChain *swapchainObj;
};

#endif // VULKANRENDERER_H
