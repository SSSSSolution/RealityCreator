#ifndef VULKANAPPLICATION_H
#define VULKANAPPLICATION_H

#include "VulkanInstance.h"
#include "VulkanDevice.h"

class VulkanDevice;
class VulkanRenderer;

class VulkanApplication
{
public:
    static VulkanApplication* getInstance();
    ~VulkanApplication();

    void initialize();
    void prepare();
    void render();


private:
    VulkanApplication();

    VkResult createVulkanInstance(std::vector<const char *>layers,
                                  std::vector<const char *>extensions,
                                  const char *appName);
    VkResult createVulkanDevice(std::vector<const char *> layers,
                                std::vector<const char *> extensions);
    VkResult createVulkanRenderer();

public:
    VulkanInstance vulkanInstance;
    VulkanDevice   vulkanDevice;
    VulkanRenderer *vulkanRenderer;

private:
    static std::unique_ptr<VulkanApplication> instance;
    static std::once_flag onlyOnce;

    std::vector<VkCommandBuffer> drawCmdList;
};

#endif // VULKANAPPLICATION_H
