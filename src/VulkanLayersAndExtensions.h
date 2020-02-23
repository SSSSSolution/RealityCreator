#ifndef VULKANLAYERSANDEXTENSIONS_H
#define VULKANLAYERSANDEXTENSIONS_H

#include "headers.h"

struct  LayerProperties {
    VkLayerProperties properties;
    std::vector<VkExtensionProperties> extensions;
};

class VulkanLayersAndExtensions
{
public:
    VulkanLayersAndExtensions();
    ~VulkanLayersAndExtensions();

    VkResult getInstanceLayerProperties();
    VkResult getExtensionProperties(LayerProperties &layerProps);

    VkResult createDebugReportCallback();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugFunction(VkFlags msgFlags,
                                                        VkDebugReportObjectTypeEXT objType,
                                                        uint64_t srcObject,
                                                        size_t location,
                                                        int32_t msgCode,
                                                        const char *layerPrefix,
                                                        const char *msg,
                                                        void *userData);

public:
    std::vector<const char *> instanceLayers;
    std::vector<const char *> instanceExtensions;
    std::vector<const char *> appRequestedLayerNames;
    std::vector<const char *> appRequestedExtentionNames;
    std::vector<LayerProperties> layerPropertyList;

    VkDebugReportCallbackCreateInfoEXT dbgReportCreateInfo = {};

private:
    PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallback;
    PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallback;
    VkDebugReportCallbackEXT vkDebugReportCallback;
};

#endif // VULKANLAYERSANDEXTENSIONS_H
