#include "VulkanInstance.h"

VulkanInstance::VulkanInstance()
{

}

VulkanInstance::~VulkanInstance()
{

}

VkResult VulkanInstance::createInstance(std::vector<const char *> layers,
                                        std::vector<const char *> extensions,
                                        const char * appName)
{
     layerExtension.instanceLayers = layers;
     layerExtension.instanceExtensions = extensions;

     VkApplicationInfo appInfo = {};
     appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
     appInfo.pNext = nullptr;
     appInfo.pApplicationName = appName;
     appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
     appInfo.pEngineName = appName;
     appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
     appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

     VkInstanceCreateInfo instInfo = {};
     instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
     instInfo.pNext = &layerExtension.dbgReportCreateInfo;
     instInfo.flags = 0;
     instInfo.pApplicationInfo = &appInfo;

     instInfo.enabledLayerCount = (uint32_t)layers.size();
     instInfo.ppEnabledLayerNames = (layers.size() == 0) ? nullptr : layers.data();

     instInfo.enabledExtensionCount = (uint32_t)extensions.size();
     instInfo.ppEnabledExtensionNames = (extensions.size() == 0) ? nullptr : extensions.data();

     VkResult ret = vkCreateInstance(&instInfo, nullptr, &vkInstance);
     assert(ret == VK_SUCCESS);
     return ret;
}

void VulkanInstance::destroyInstance()
{
    vkDestroyInstance(vkInstance, nullptr);
}
