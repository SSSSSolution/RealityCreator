#ifndef VULKANAPPLICATION_H
#define VULKANAPPLICATION_H

#include "VulkanInstance.h"

class VulkanApplication
{
public:
    static VulkanApplication* getInstance();
    ~VulkanApplication();

    VkResult createVulkanInstance(std::vector<const char *>layers,
                                  std::vector<const char *>extensions,
                                  const char *appName);
    VkResult createDebugReportCallback();

private:
    VulkanApplication();

public:
    VulkanInstance vulkanInstance;

private:
    static std::unique_ptr<VulkanApplication> instance;
    static std::once_flag onlyOnce;
};

#endif // VULKANAPPLICATION_H
