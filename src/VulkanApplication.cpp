#include "VulkanApplication.h"
#include <assert.h>


extern std::vector<const char *> instanceExtensionNames;
extern std::vector<const char *> layerNames;
extern std::vector<const char *> deviceExtensionNames;

VulkanApplication::VulkanApplication()
{
    instanceObj.layerExtension.getInstanceLayerProperties();

    deviceObj = nullptr;
    rendererObj = nullptr;
    debugFlag = false;
}

VulkanApplication::~VulkanApplication()
{
    delete rendererObj;
}

VulkanApplication *VulkanApplication::getInstance()
{
    std::call_once(onlyOnce, [](){instance.reset(new VulkanApplication()); });
    return instance.get();
}


VkResult VulkanApplication::createVulkanInstance(std::vector<const char *> &layers,
                                                 std::vector<const char *> &extensions,
                                                 const char *appNames)
{
    return  instanceObj.createInstance(layers, extensions, appNames);
}


VkResult VulkanApplication::handShakeWithDevice(VkPhysicalDevice *gpu,
                                                std::vector<const char *> &layers,
                                                std::vector<const char *> &extensions)
{
    deviceObj = new VulkanDevice(gpu);
    if (!deviceObj)
        return VK_ERROR_OUT_OF_HOST_MEMORY;

    deviceObj->layerExtension.getDeviceExtensionProperties(gpu);

    vkGetPhysicalDeviceMemoryProperties(*gpu, &deviceObj->memoryProps);

    deviceObj->getPhysicalDeviceQueueAndProperties();

    deviceObj->getGraphicsQueueHandle();

    return deviceObj->createDevice(layers, extensions);
}


VkResult VulkanApplication::enumeratePhysicalDevices(std::vector<VkPhysicalDevice> &gpus)
{
    uint32_t gpuDeviceCount;

    VkResult result = vkEnumeratePhysicalDevices(instanceObj.instance,
                                                 &gpuDeviceCount, nullptr);
    assert(result == VK_SUCCESS);

    assert(gpuDeviceCount);

    gpus.resize(gpuDeviceCount);

    result = vkEnumeratePhysicalDevices(instanceObj.instance,
                                        &gpuDeviceCount, gpus.data());
    assert(result == VK_SUCCESS);

    return result;
}

void VulkanApplication::initialize()
{
    char title[] = "Hello World!";

    instanceObj.layerExtension.areLayersSupported(layerNames);

    createVulkanInstance(layerNames, instanceExtensionNames, title);

//    if (debugFlag)
//        instanceObj.layerExtension.createDebugReportCallback();

    std::vector<VkPhysicalDevice> gpuList;
    enumeratePhysicalDevices(gpuList);

    if (gpuList.size() > 0)
        handShakeWithDevice(&gpuList[0], layerNames, deviceExtensionNames);

//    renderObj = new VulkanRenderer(this, deviceObj);
//    rendererObj->initialize();
}


































