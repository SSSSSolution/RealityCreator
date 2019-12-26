#ifndef VULKANLAYERSANDEXTENSIONS_H
#define VULKANLAYERSANDEXTENSIONS_H

#include <vulkan/vulkan.h>
#include <vector>
#include <QVulkanInstance>
#include <QVulkanFunctions>
#include <QVulkanDeviceFunctions>

/**
 * 获取vulkan的layer属性和每个layer对应的extension的属性
 */

struct LayerProperties {
    VkLayerProperties properties;
    std::vector<VkExtensionProperties> extensions;
};

class VulkanLayersAndExtensions
{
public:
    VulkanLayersAndExtensions(QVulkanInstance *inst, VkPhysicalDevice gpu);
    VkBool32 areLayerSupported(std::vector<const char*> &layerNames);
    VkResult createDebugReportCallback();
    VKAPI_ATTR static VkBool32 VKAPI_CALL debugFunction(
            VkFlags msgFlags, VkDebugReportObjectTypeEXT objType,
            uint64_t srcObject, size_t location, int32_t msgCode,
            const char * pLayerPrefix, const char *pMsg, void *pUserData);

private:
    VkResult getInstanceLayerProperties();
    VkResult getExtensionPropertiesByLayer(LayerProperties &layerProps,
                                           VkPhysicalDevice *gpu = nullptr);
private:
    QVulkanInstance *m_inst;
    VkPhysicalDevice m_gpu;
    QVulkanFunctions *m_vulkanFuncs;
    std::vector<LayerProperties> layerPropertiesList;
};

#endif // VULKANLAYERSANDEXTENSIONS_H


























