#include "VulkanLayerAndExtension.h"
#include "VulkanApplication.h"
#include <string.h>
#include <algorithm>
#include <iostream>

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
        if (gpu)
            result = vkEnumerateDeviceExtensionProperties(*gpu, layerName, &extensionCount, nullptr);
        else
            result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, nullptr);

        if (result != VK_SUCCESS || extensionCount == 0)
            continue;

        layerProps.extensions.resize(extensionCount);

        if (gpu)
            result = vkEnumerateDeviceExtensionProperties(*gpu, layerName, &extensionCount, layerProps.extensions.data());
        else
            result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, layerProps.extensions.data());
    } while(result == VK_INCOMPLETE);

    return result;
}

VkResult VulkanLayerAndExtension::getDeviceExtensionProperties(VkPhysicalDevice *gpu)
{
    VkResult result;

    // Query all the extesions for each layer and store it.
    std::cout << "Device extensions" << std::endl;
    std::cout << "=================" << std::endl;
    VulkanApplication *appObj = VulkanApplication::getInstance();
    std::vector<LayerProperties> *instanceLayerProp = &appObj->getInstance()->instanceObj.layerExtension.layerPropertyList;
    for (auto globalLayerProp : *instanceLayerProp)
    {
        LayerProperties layerProps;
        layerProps.properties = globalLayerProp.properties;

        if ((result = getExtensionProperties(layerProps, gpu)) != VK_SUCCESS)
            continue;

        std::cout << "\n" << globalLayerProp.properties.description <<  "\n\t|\n\t|---[Layer Name]--> "
                  << globalLayerProp.properties.layerName << "\n";
        layerPropertyList.push_back(layerProps);

        if (layerProps.extensions.size() != 0) {
            for (auto j : layerProps.extensions) {
                std::cout <<  "\t\t|\n\t\t|---[Device Extesion]--> " << j.extensionName << "\n";
            }
        } else {
            std::cout << "\t\t|\n\t\t|---[Device Extesion]--> No extension found \n";
        }
    }
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















































