#include "VulkanApplication.h"
#include "VulkanRenderer.h"
#include "VulkanSwapChain.h"
#include "CommandBufferMgr.h"

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


void VulkanApplication::initialize()
{
    VkResult ret;
    ret = createVulkanInstance(layers, extensions, "hello world");
    assert(ret == VK_SUCCESS);

    // if debug
    vulkanInstance.layerExtension.createDebugReportCallback();

    ret = createVulkanDevice(deviceLayers, deviceExtensions);
    assert(ret == VK_SUCCESS);

    createVulkanRenderer();
}

void VulkanApplication::prepare()
{
    vulkanRenderer->prepare();
}

void VulkanApplication::render()
{
    vulkanRenderer->render();
}

VkResult VulkanApplication::createVulkanDevice(std::vector<const char *> layers,
                                               std::vector<const char *> extensions)
{
    std::vector<VkPhysicalDevice> gpuList;
    VulkanDevice::enumeratePhysicalDevice(gpuList);

    vulkanDevice.setVkPhysicalDevice(gpuList.at(0));
    return vulkanDevice.createDevice(layers, extensions);
}

VkResult VulkanApplication::createVulkanRenderer()
{
    vulkanRenderer = new VulkanRenderer();
    vulkanRenderer->initialize();

    return VK_SUCCESS;
}



























