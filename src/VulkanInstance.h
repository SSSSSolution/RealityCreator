#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#include <vulkan/vulkan.h>
#include <vector>

class VulkanInstance
{
public:
    VulkanInstance();
    ~VulkanInstance() {}
    VkResult create(std::vector<const char *> &layers,
                    std::vector<const char *> &extensions);
    VkInstance getVkInstance();
    VkResult createDebugReportCallback();

private:
    VkInstance instance;
    VkDebugReportCallbackCreateInfoEXT dbgReportInfo = {};
};

#endif // VULKANINSTANCE_H
