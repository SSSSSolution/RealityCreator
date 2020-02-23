#include "VulkanLayersAndExtensions.h"
#include <QApplication>

#include "VulkanApplication.h"
#include "VulkanDevice.h"

static std::vector<const char *> layers = {
    "VK_LAYER_LUNARG_api_dump",
    "VK_LAYER_LUNARG_standard_validation"
};

static std::vector<const char *> extensions = {
    VK_KHR_SURFACE_EXTENSION_NAME,
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
    VK_EXT_DEBUG_REPORT_EXTENSION_NAME
};

static std::vector<const char *> deviceLayers = {
};

static std::vector<const char *> deviceExtensions = {
};

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    VulkanLayersAndExtensions layersAndExtensions;
    layersAndExtensions.getInstanceLayerProperties();

    VulkanApplication::getInstance()->createVulkanInstance(layers, extensions, nullptr);
    assert(VulkanApplication::getInstance()->createDebugReportCallback() == VK_SUCCESS);

    std::vector<VkPhysicalDevice> gpuList;
    VulkanDevice::enumeratePhysicalDevice(gpuList);

    VulkanDevice vulkanDevice;
    vulkanDevice.setVkPhysicalDevice(gpuList.at(0));
    vulkanDevice.createDevice(deviceLayers, deviceExtensions);
}
