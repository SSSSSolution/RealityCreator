#include "VulkanDevice.h"

VulkanDevice::VulkanDevice(VkDevice device, VkPhysicalDevice physicalDevice)
    : m_vkDevice(device), m_physical_device(physicalDevice)
{

}

VkDevice VulkanDevice::getVkDevice() const
{
    return m_vkDevice;
}

VkPhysicalDevice VulkanDevice::getPhysicalDevice() const
{
    return m_physical_device;
}
