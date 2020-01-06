#include "VulkanLayerAndExtension.h"
#include <string.h>
#include <algorithm>

VulkanLayerAndExtension::VulkanLayerAndExtension()
{
    dbgCreateDebugReportCallback = nullptr;
    dbgDestroyDebugReportCallback = nullptr;
    debugReportCallback = nullptr;
}

VkResult VulkanLayerAndExtension::getInstanceLayerProperties()
{
    uint32_t instanceLayerCount;
    std::vector<VkLayerProperties> layerProperties;
    VkResult result;

    do {
        result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);

        if (result != VK_SUCCESS)
            return result;

        if (instanceLayerCount == 0)
            return VK_INCOMPLETE;

        layerProperties.resize(instanceLayerCount);
        result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerProperties.data());
    } while (result == VK_INCOMPLETE);

    return result;
}

VkResult VulkanLayerAndExtension::getExtensionProperties(LayerProperties &layerProps, VkPhysicalDevice *gpu)
{
    uint32_t extensionCount;
    VkResult result;
    char *layerName = layerProps.properties.layerName;

    do {
        result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, nullptr);

        if (result != VK_SUCCESS || extensionCount == 0)
            continue;

        layerProps.extensions.resize(extensionCount);

        result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, layerProps.extensions.data());
    } while(result == VK_INCOMPLETE);

    return result;
}

VkBool32 VulkanLayerAndExtension::areLayersSupported(std::vector<const char *> &layerNames)
{
    uint32_t checkCount = (uint32_t)layerNames.size();
    uint32_t layerCount = (uint32_t)layerPropertyList.size();
    std::vector<const char *>unsupportedLayerNames;

    for (uint32_t i = 0; i < checkCount; i++) {
        VkBool32 isSupported = VK_FALSE;
        for (uint32_t j = 0; j < layerCount; j++) {
            if (!strcmp(layerNames[i], layerPropertyList[j].properties.layerName)) {
                isSupported = VK_TRUE;
            }
        }

        if (!isSupported) {
            unsupportedLayerNames.push_back(layerNames[i]);
        } else {

        }
    }

    for (auto i : unsupportedLayerNames) {
        auto it = std::find(layerNames.begin(), layerNames.end(), i);
        if (it != layerNames.end()) layerNames.erase(it);
    }

    return true;
}















































