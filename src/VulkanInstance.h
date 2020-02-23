#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#include "headers.h"
#include "VulkanLayersAndExtensions.h"

class VulkanInstance
{
public:
    VulkanInstance();
    ~VulkanInstance();

    VkResult createInstance(std::vector<const char *> layers,
                            std::vector<const char *> extensions,
                            const char * appNames);
    void destroyInstance();

public:
    VkInstance vkInstance;
    VulkanLayersAndExtensions layerExtension;
//    VkPhysicalDevice vkPhysicalDevice;
//    VkDevice vkDevice;
};

#endif // VULKANINSTANCE_H
