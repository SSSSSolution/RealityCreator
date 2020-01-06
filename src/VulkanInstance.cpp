#include "VulkanInstance.h"
#include <assert.h>

VkResult VulkanInstance::createInstance(std::vector<const char *> &layers,
                                        std::vector<const char *> &extensionNames,
                                        const char *appName)
{
    layerExtension.appRequestedLayerNames = layers;
    layerExtension.appRequestedExtensionNames = extensionNames;

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = appName;
    appInfo.engineVersion = 1;
    appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

    // Define the vulkan instance create info structure
    VkInstanceCreateInfo instInfo = {};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pNext = nullptr;
    instInfo.flags = 0;
    instInfo.pApplicationInfo = &appInfo;

    // Specify the list of layer name to be enabled.
    instInfo.enabledLayerCount = (uint32_t)layers.size();
    instInfo.ppEnabledLayerNames = layers.size() ? layers.data() : nullptr;

    // Specify the list of extensions to be used in the application.
    instInfo.enabledExtensionCount = (uint32_t)extensionNames.size();
    instInfo.ppEnabledExtensionNames = extensionNames.size() ? extensionNames.data() : nullptr;

    VkResult ret = vkCreateInstance(&instInfo, nullptr, &instance);
    assert(ret == VK_SUCCESS);

    return ret;

}

void VulkanInstance::destroyInstance()
{
    vkDestroyInstance(instance, nullptr);
}
















