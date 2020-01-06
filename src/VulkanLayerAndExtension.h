#ifndef VULKANLAYERSANDEXTENSIONS_H
#define VULKANLAYERSANDEXTENSIONS_H

#include <vulkan/vulkan.h>
#include <vector>

struct LayerProperties {
    VkLayerProperties properties;
    std::vector<VkExtensionProperties> extensions;
};

class VulkanLayerAndExtension
{
public:
    VulkanLayerAndExtension();
    ~VulkanLayerAndExtension() {}

    VkResult getInstanceLayerProperties();

    VkResult getExtensionProperties(LayerProperties &layerProps,
                                    VkPhysicalDevice *gpu = nullptr);

    VkResult getDeviceExtensionProperties(VkPhysicalDevice *gpu);

    VkBool32 areLayersSupported(std::vector<const char *> &layerNames);
//    VkResult createDebugReportCallback();
//    void destroyDebugReportCallback();


    static VKAPI_ATTR VkBool32 VKAPI_CALL debugFunction(VkFlags msgFlags,
                                                        VkDebugReportObjectTypeEXT objType,
                                                        uint64_t srcObject,
                                                        size_t location,
                                                        int32_t msgCode,
                                                        const char *layerPrefix,
                                                        const char *msg,
                                                        void *userData);

public:
    std::vector<const char *> appRequestedLayerNames;

    std::vector<const char *> appRequestedExtensionNames;

    std::vector<LayerProperties> layerPropertyList;

private:
    PFN_vkCreateDebugReportCallbackEXT dbgCreateDebugReportCallback;
    PFN_vkDestroyDebugReportCallbackEXT dbgDestroyDebugReportCallback;
    VkDebugReportCallbackEXT debugReportCallback;

    VkDebugReportCallbackCreateInfoEXT dbgReportCreateInfo = {};
};



#endif // VULKANLAYERSANDEXTENSIONS_H



























