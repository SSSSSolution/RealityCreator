#include "VulkanApplication.h"

std::unique_ptr<VulkanApplication> VulkanApplication::instance;
std::once_flag VulkanApplication::onlyOnce;

VulkanApplication::VulkanApplication()
{

}

VulkanApplication::~VulkanApplication()
{

}

VulkanApplication *VulkanApplication::getInstance()
{
    std::call_once(onlyOnce, [](){instance.reset(new VulkanApplication()); });
    return instance.get();
}

VkResult VulkanApplication::createVulkanInstance(std::vector<const char *> layers,
                                                 std::vector<const char *> extensions,
                                                 const char *appName)
{
    return vulkanInstance.createInstance(layers, extensions, appName);
}

VkResult VulkanApplication::createDebugReportCallback()
{
    return vulkanInstance.layerExtension.createDebugReportCallback();
}
