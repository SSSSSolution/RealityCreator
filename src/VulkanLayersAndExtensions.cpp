#include "VulkanLayersAndExtensions.h"

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





















































