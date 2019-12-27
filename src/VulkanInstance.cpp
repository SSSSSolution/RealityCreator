#include "VulkanInstance.h"
#include <assert.h>
#include <QDebug>
#include <iostream>
VulkanInstance::VulkanInstance()
{

}

VkResult VulkanInstance::create(std::vector<const char *> &layers,
                                std::vector<const char *> &extensions)
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "App Name";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Reality";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    // Define the Vulkan instance create info structure
    VkInstanceCreateInfo instInfo = {};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pNext = &dbgReportInfo;
    instInfo.flags = 0;
    instInfo.pApplicationInfo = &appInfo;

    instInfo.enabledLayerCount = (uint32_t)layers.size();
    instInfo.ppEnabledLayerNames = layers.size() ? layers.data() : nullptr;

    instInfo.enabledExtensionCount = (uint32_t)extensions.size();
    instInfo.ppEnabledExtensionNames = extensions.size() ? extensions.data() : nullptr;

    VkResult result = vkCreateInstance(&instInfo, nullptr, &instance);
    assert(result == VK_SUCCESS);

    return result;
}

VkInstance VulkanInstance::getVkInstance()
{
    return instance;
}

VkResult VulkanInstance::createDebugReportCallback()
{
    VkResult ret;

    PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)
    vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)
    vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    assert(vkCreateDebugReportCallbackEXT != nullptr);
    assert(vkDestroyDebugReportCallbackEXT != nullptr);

    dbgReportInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    dbgReportInfo.pfnCallback = debugFunction;
    dbgReportInfo.pUserData = nullptr;
    dbgReportInfo.pNext = nullptr;
    dbgReportInfo.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT |
                          VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                          VK_DEBUG_REPORT_ERROR_BIT_EXT |
                          VK_DEBUG_REPORT_DEBUG_BIT_EXT |
                          VK_DEBUG_REPORT_INFORMATION_BIT_EXT;

    ret = vkCreateDebugReportCallbackEXT(instance, &dbgReportInfo, NULL, &m_debugReportCallback);
    assert(ret == VK_SUCCESS);
    return ret;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanInstance::debugFunction(VkFlags msgFlags,
                              VkDebugReportObjectTypeEXT objType,
                              uint64_t srcObject,
                              size_t location,
                              int32_t msgCode,
                              const char *layerPrefix,
                              const char *msg,
                              void *userData)
{
    if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        qDebug() << "[VK_DEBUG_REPORT] ERROR: [" << layerPrefix
                 << "] Code" << msgCode << ":" << msg;
    } else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        qDebug() << "[VK_DEBUG_REPORT] WARNING: [" << layerPrefix
                 << "] Code" << msgCode << ":" << msg;
    } else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
        qDebug() << "[VK_DEBUG_REPORT] INFORMATION: [" << layerPrefix
                 << "] Code" << msgCode << ":" << msg;
    } else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        qDebug() << "[VK_DEBUG_REPORT] PERFORMANCE: [" << layerPrefix
                 << "] Code" << msgCode << ":" << msg;
    } else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
        qDebug() << "[VK_DEBUG_REPORT] DEBUG: [" << layerPrefix
                 << "] Code" << msgCode << ":" << msg;
    } else {
        return VK_FALSE;
    }

    return VK_TRUE;
}




















