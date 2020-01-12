#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include "headers.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"

class VulkanSwapChain;
class VulkanApplication;
class VulkanDrawable;
class VulkanDevice;

#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

class VulkanRenderer
{
public:
    VulkanRenderer(VulkanApplication *appObj, VulkanDevice *deviceObj);
    ~VulkanRenderer();

    // simple life cycle
    void initialize();
    void prepare();
    bool render();

    // Create an empty window
    void createPresentationWindow(const int &windowWidth = 500, const int &windowHeight = 500);
    void setImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout,
                        VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask, const VkCommandBuffer &cmdBuf);

    // Destroy the presentation window
    void destroyPresentationWindow();

    // Getter functions for member variable specific to classes
    inline VulkanApplication *getApplication() { return appObj; }
    inline VulkanDevice *getDevice() { return deviceObj; }
    inline VulkanSwapChain* getSwapChain() { return swapChainObj; }
    inline std::vector<VulkanDrawable*> *getDrawingItems() { return &drawableList; }
    inline VkCommandPool *getCommandPool() { return &cmdPool; }
    inline VulkanShader *getShader() { return &shaderObj; }
    inline VulkanPipeline *getPipelineObj() { return &pipelineObj; }

    void createCommandPool();
    void buildSwapChainAndDepthImage();
    void createDepthImage();
    void createVertexBuffer();
    void createRenderPass(bool includeDepth, bool clear = true);
    void createFrameBuffer(bool includeDepth);
    void createShaders();
    void createPipelineStateManagement();

    void destroyCommandBuffer();
    void destroyCommandPool();
    void destroyDepthBuffer();
    void destroyRenderpass();
    void destroyFramebuffers();
    void destroyPipeline();
    void destroyDrawableCommandBuffer();
    void destroyDrawableSynchronizationObjects();

public:
#ifdef _WIN32
    HINSTANCE connection;
    char name[APP_NAME_STR_LEN];
    HWND window;
#else
#endif // _WIN32
    struct Depth {
        VkFormat format;
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    } depth;

    VkCommandBuffer cmdDepthImage;
    VkCommandPool cmdPool;
    VkCommandBuffer cmdVertexBuffer;

    VkRenderPass renderPass;
    std::vector<VkFramebuffer> framebuffers;
    std::vector<VkPipeline*> pipelineList;

    int width, height;

private:
    VulkanApplication *appObj;
    VulkanDevice *deviceObj;
    VulkanSwapChain *swapChainObj;
    VulkanShader shaderObj;
    VulkanPipeline pipelineObj;
    std::vector<VulkanDrawable *> drawableList;
};

#endif // VULKANRENDERER_H




































