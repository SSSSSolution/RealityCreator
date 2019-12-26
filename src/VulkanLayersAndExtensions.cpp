#include "VulkanLayersAndExtensions.h"
#include <algorithm>

VulkanLayersAndExtensions::VulkanLayersAndExtensions(QVulkanInstance *inst, VkPhysicalDevice gpu)
    : m_inst(inst), m_gpu(gpu)
{

    m_vulkanFuncs = inst->functions();

    getInstanceLayerProperties();
}

VkResult VulkanLayersAndExtensions::getInstanceLayerProperties()
{
    uint32_t instanceLayerCount;
    std::vector<VkLayerProperties> layerProperties;
    VkResult ret;

    // Query all layers
    do {
        ret = m_vulkanFuncs->vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
        if (ret != VK_SUCCESS) {
            qDebug("==============1");
            return ret;
        }

        if (instanceLayerCount == 0) {
            qDebug("==============2");
            return VK_INCOMPLETE;
        }

        layerProperties.resize(instanceLayerCount);
        ret =  m_vulkanFuncs->vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerProperties.data());
    } while (ret == VK_INCOMPLETE);

    // Query all the extensions for each layer and store it.
    qDebug() << "\nInstanced Layers";
    qDebug() << "==================";
    for (auto layerProp : layerProperties) {
        qDebug() << "\n" << layerProp.description   <<
                    "\n\t|\n\t|---[Layer Name]--> " <<
                    layerProp.layerName;

        LayerProperties layerProps;
        layerProps.properties = layerProp;

        // get Instance level extension for corresponding layer properties
        ret = getExtensionPropertiesByLayer(layerProps);

        if (ret != VK_SUCCESS) {
            continue;
        }

        layerPropertiesList.push_back(layerProps);

        for (auto j : layerProps.extensions) {
            qDebug() << "\t\t|\n\t\t|--- [Layer Extension]-->"
                     << j.extensionName;
        }

    }
}

VkBool32 VulkanLayersAndExtensions::areLayerSupported(std::vector<const char *> &layerNames)
{
    uint32_t checkCount = layerNames.size();
    uint32_t layerCount = layerPropertiesList.size();
    std::vector<const char *> unsupportLayerNames;

    for (uint32_t i = 0; i < checkCount; i++) {
        VkBool32 isSupported = 0;
        for (uint32_t j = 0; j < layerCount; j++) {
            if (!strcmp(layerNames[i], layerPropertiesList[j].properties.layerName)) {
                isSupported = 1;
            }
        }

        if (!isSupported) {
            qDebug() << "No Layer support found, removed from layer: "
                     << layerNames[i];
            unsupportLayerNames.push_back(layerNames[i]);
        } else {
            qDebug() << "Layer supported: " << layerNames[i];
        }
    }

    for (auto i : unsupportLayerNames) {
        auto it = std::find(layerNames.begin(), layerNames.end(), i);
        if (it != layerNames.end())
            layerNames.erase(it);
    }

    return 1;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanLayersAndExtensions::debugFunction(
        VkFlags msgFlags, VkDebugReportObjectTypeEXT objType,
        uint64_t srcObject, size_t location, int32_t msgCode,
        const char * pLayerPrefix, const char *pMsg, void *pUserData)
{
    if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
        qDebug() << "[VK_DEBUG_REPORT] ERROR: [" << pLayerPrefix
                 << "] Code" << msgCode << ":" << pMsg;
    } else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        qDebug() << "[VK_DEBUG_REPORT] WARNING: [" << pLayerPrefix
                 << "] Code" << msgCode << ":" << pMsg;
    } else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
        qDebug() << "[VK_DEBUG_REPORT] INFORMATION: [" << pLayerPrefix
                 << "] Code" << msgCode << ":" << pMsg;
    } else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        qDebug() << "[VK_DEBUG_REPORT] PERFORMANCE: [" << pLayerPrefix
                 << "] Code" << msgCode << ":" << pMsg;
    } else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
        qDebug() << "[VK_DEBUG_REPORT] DEBUG: [" << pLayerPrefix
                 << "] Code" << msgCode << ":" << pMsg;
    } else {
        return VK_FALSE;
    }
    return VK_TRUE;
}

VkResult VulkanLayersAndExtensions::getExtensionPropertiesByLayer(LayerProperties &layerProps,
                                                                  VkPhysicalDevice *gpu)
{
    uint32_t extensionCount;
    VkResult ret;

    char *layerName = layerProps.properties.layerName;

    do {
        // Get the total number of extension in this layer
        if (gpu) {
            ret = m_vulkanFuncs->vkEnumerateDeviceExtensionProperties(
                        *gpu, layerName, &extensionCount, NULL);
        } else {
            ret = m_vulkanFuncs->vkEnumerateInstanceExtensionProperties(
                        layerName, &extensionCount, NULL);
        }
        if (ret != VK_SUCCESS || extensionCount == 0)
            continue;

        // Gather all extension properties
        layerProps.extensions.resize(extensionCount);
        if (gpu) {
            ret = m_vulkanFuncs->vkEnumerateDeviceExtensionProperties
                                (*gpu, layerName, &extensionCount,
                                layerProps.extensions.data());
        } else {
            ret = m_vulkanFuncs->vkEnumerateInstanceExtensionProperties
                                (layerName, &extensionCount,
                                layerProps.extensions.data());
        }
    } while(ret == VK_INCOMPLETE);

    return ret;
}





















































