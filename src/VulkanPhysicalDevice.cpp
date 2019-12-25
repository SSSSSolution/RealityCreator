#include "VulkanPhysicalDevice.h"
#include <string>
#include <QDebug>

VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice gpu)
    : m_gpu(gpu)
{
    // Get gpu properties
    vkGetPhysicalDeviceProperties(m_gpu, &m_props);
    vkGetPhysicalDeviceMemoryProperties(m_gpu, &m_memProps);

    uint32_t queueFamilyPropertyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &queueFamilyPropertyCount, nullptr);
    m_queueFamilyPropsList.resize(queueFamilyPropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &queueFamilyPropertyCount, m_queueFamilyPropsList.data());
    /*
    typedef struct VkQueueFamilyProperties {
        VkQueueFlags    queueFlags;
        uint32_t        queueCount;
        uint32_t        timestampValidBits;
        VkExtent3D      minImageTransferGranularity;
    } VkQueueFamilyProperties;
    */
    qDebug() << "queue family property:";
    qDebug() << "======================";
    bool find = false;
    uint32_t index = 0;
    for (auto queueFamilyProperties : m_queueFamilyPropsList) {
        if (queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            m_graphicQueueFamilyIndex =index;
            find = true;
        }
        index++;
        qDebug() << "flags: " << queueFamilyProperties.queueFlags;
        qDebug() << "count: " << queueFamilyProperties.queueCount;
    }
    assert(find);

}

std::vector<VulkanPhysicalDevice> VulkanPhysicalDevice::enumeratePhysicalDevices(VkInstance inst)
{
    uint32_t gpuDeviceCount;
    std::vector<VkPhysicalDevice> physicalDeviceList;
    std::vector<VulkanPhysicalDevice> gpuList;

    vkEnumeratePhysicalDevices(inst, &gpuDeviceCount, nullptr);
    physicalDeviceList.resize(gpuDeviceCount);
    vkEnumeratePhysicalDevices(inst, &gpuDeviceCount, physicalDeviceList.data());

    for (const auto &physicalDevice : physicalDeviceList) {
        gpuList.push_back(VulkanPhysicalDevice(physicalDevice));
    }

    return gpuList;
}

VulkanDevice VulkanPhysicalDevice::createLogicalDevice(std::vector<const char*>& layers,
                                                       std::vector<const char*>& extensions)
{
    VkDevice device;
    VkResult ret;
    float queuePriorities[1] = { 0.0f };
    // Create the object information
    VkDeviceQueueCreateInfo queueInfo = {};
    queueInfo.queueFamilyIndex = m_graphicQueueFamilyIndex;
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = nullptr;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queuePriorities;

    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = nullptr;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    // device layers are deprecated
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = nullptr;
    deviceInfo.enabledExtensionCount = extensions.size();
    deviceInfo.ppEnabledExtensionNames = extensions.data();
    deviceInfo.pEnabledFeatures = nullptr;

    ret = vkCreateDevice(m_gpu, &deviceInfo, nullptr, &device);
    assert(ret == VK_SUCCESS);
    return VulkanDevice(device);
}

VkPhysicalDeviceProperties VulkanPhysicalDevice::getProperties() const
{
    return m_props;
}

std::string VulkanPhysicalDevice::getGpuName() const
{
    return std::string(m_props.deviceName);
}

std::string VulkanPhysicalDevice::getApiVersion() const
{
    std::string major = std::to_string(VK_VERSION_MAJOR(m_props.apiVersion));
    std::string minor = std::to_string(VK_VERSION_MINOR(m_props.apiVersion));
    std::string patch = std::to_string(VK_VERSION_PATCH(m_props.apiVersion));

    return major + "." + minor + "." + patch;
}





















