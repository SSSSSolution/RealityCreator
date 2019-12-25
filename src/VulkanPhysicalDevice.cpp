#include "VulkanPhysicalDevice.h"
#include <string>

VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice gpu)
    : m_gpu(gpu)
{
    // Get gpu properties
    vkGetPhysicalDeviceProperties(m_gpu, &m_props);
    vkGetPhysicalDeviceMemoryProperties(m_gpu, &m_memProps);
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

//VulkanDevice VulkanPhysicalDevice::createLogicalDevice()
//{
//    VKResult ret;

//    // Create the object information
//    VkDeviceQueueCreateInfo queueInfo = {};
//    queueInfo.queueFamilyIndex
//    ret = vkCreateDevice(m_gpu, )
//}

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





















