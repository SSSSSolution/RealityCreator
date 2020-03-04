#include "VulkanLayersAndExtensions.h"
#include "VulkanApplication.h"

VulkanLayersAndExtensions::VulkanLayersAndExtensions()
{

}

VulkanLayersAndExtensions::~VulkanLayersAndExtensions()
{

}

VkResult VulkanLayersAndExtensions::getInstanceLayerProperties()
{
    uint32_t instanceLayerCount;
    std::vector<VkLayerProperties> layerProperties;
    VkResult ret;

    ret = vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
    assert(ret == VK_SUCCESS);
    qDebug() << __func__ << "instanceLayerCount: " << instanceLayerCount;

    layerProperties.resize(instanceLayerCount);
    ret = vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerProperties.data());
    assert(ret == VK_SUCCESS);

    layerPropertyList.resize(instanceLayerCount);
    qDebug() << "Enumerate instance layer: ";
    for (uint32_t i = 0; i < layerPropertyList.size(); ++i)
    {
        qDebug() << "\t" << layerProperties.at(i).layerName;
        layerPropertyList.at(i).properties = layerProperties.at(i);
        getExtensionProperties(layerPropertyList.at(i));
    }
    return ret;
}

VkResult VulkanLayersAndExtensions::getExtensionProperties(LayerProperties &layerProps)
{
    VkResult ret;
    uint32_t extensionCount;

    ret = vkEnumerateInstanceExtensionProperties(layerProps.properties.layerName, &extensionCount, nullptr);
    assert(ret == VK_SUCCESS);

    layerProps.extensions.resize(extensionCount);
    ret = vkEnumerateInstanceExtensionProperties(layerProps.properties.layerName, &extensionCount, layerProps.extensions.data());
    assert(ret == VK_SUCCESS);

    for (auto extension : layerProps.extensions)
    {
        qDebug() << "\t\t==>" << extension.extensionName;
    }
    return ret;
}

VkResult VulkanLayersAndExtensions::createDebugReportCallback()
{
    VkResult ret;

    VulkanApplication *appObj = VulkanApplication::getInstance();
    VkInstance *instance = &appObj->vulkanInstance.vkInstance;

    // Get vkCreateDebugReportCallbackEXT API
    vkCreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(*instance, "vkCreateDebugReportCallbackEXT");
    assert(vkCreateDebugReportCallback != nullptr);

    vkDestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(*instance, "vkDestroyDebugReportCallbakcEXT");
    assert(vkCreateDebugReportCallback != nullptr);

    dbgReportCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    dbgReportCreateInfo.pfnCallback = debugFunction;
    dbgReportCreateInfo.pUserData = nullptr;
    dbgReportCreateInfo.pNext = nullptr;
    dbgReportCreateInfo.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                                VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;

    ret = vkCreateDebugReportCallback(*instance, &dbgReportCreateInfo, nullptr, &vkDebugReportCallback);
    assert(ret == VK_SUCCESS);

    return ret;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanLayersAndExtensions::debugFunction(VkFlags msgFlags,
                                                                        VkDebugReportObjectTypeEXT objType,
                                                                        uint64_t srcObject,
                                                                        size_t location,
                                                                        int32_t msgCode,
                                                                        const char *layerPrefix,
                                                                        const char *msg,
                                                                        void *userData)
{
    if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        std::cout << "[VK_DEBUG_REPORT] ERROR: ["       << layerPrefix << "] Code" << msgCode << ":" << msg << std::endl;
    } else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        std::cout << "[VK_DEBUG_REPORT] WARNING: ["     << layerPrefix << "] Code" << msgCode << ":" << msg << std::endl;
    } else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
        std::cout << "[VK_DEBUG_REPORT] INFORMATION: [" << layerPrefix << "] Code" << msgCode << ":" << msg << std::endl;
    } else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        std::cout << "[VK_DEBUG_REPORT] PERFORMANCE: [" << layerPrefix << "] Code" << msgCode << ":" << msg << std::endl;
    } else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
        std::cout << "[VK_DEBUG_REPORT] DEBUG: ["       << layerPrefix << "] Code" << msgCode << ":" << msg << std::endl;
    } else {
        return VK_FALSE;
    }

    fflush(stdout);
    return VK_TRUE;
}
