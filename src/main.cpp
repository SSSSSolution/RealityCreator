#include "VulkanLayersAndExtensions.h"
#include <QApplication>

#include "VulkanApplication.h"
#include "VulkanDevice.h"

static std::vector<const char *> layers = {
//    "VK_LAYER_LUNARG_api_dump",
    "VK_LAYER_LUNARG_standard_validation"
};

static std::vector<const char *> extensions = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif __linux__
    VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
    VK_EXT_DEBUG_REPORT_EXTENSION_NAME
};

static std::vector<const char *> deviceLayers = {
};

static std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    VulkanLayersAndExtensions layersAndExtensions;
    layersAndExtensions.getInstanceLayerProperties();

    VulkanApplication::getInstance()->createVulkanInstance(layers, extensions, nullptr);
    assert(VulkanApplication::getInstance()->createDebugReportCallback() == VK_SUCCESS);
    VulkanApplication::getInstance()->createVulkanDevice(deviceLayers, deviceExtensions);
    VulkanApplication::getInstance()->createVulkanRenderer();

    while(1) { ; }
}
