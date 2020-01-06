#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#include <vulkan/vulkan.h>
#include <vector>
#include "VulkanLayerAndExtension.h"

class VulkanInstance
{
public:
    VulkanInstance() {}
    ~VulkanInstance() {}

    VkResult createInstance(std::vector<const char *>& layers,
                            std::vector<const char *>& extensions,
                            const char *appName);
    void destroyInstance();

public:
    VkInstance instance;
    VulkanLayerAndExtension layerExtension;
};

#endif // VULKANINSTANCE_H
