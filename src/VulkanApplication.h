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

    VkResult createVulkanInstance(std::vector<const char *>layers,
                                  std::vector<const char *>extensions,
                                  const char *appName);
    VkResult createDebugReportCallback();
    VkResult createVulkanDevice(std::vector<const char *> layers,
                                std::vector<const char *> extensions);

    void createVulkanRenderer();

private:
    VulkanApplication();

public:
    VulkanInstance vulkanInstance;
    VulkanDevice   vulkanDevice;
    VulkanRenderer *vulkanRenderer;

private:
    static std::unique_ptr<VulkanApplication> instance;
    static std::once_flag onlyOnce;
};

#endif // VULKANAPPLICATION_H
