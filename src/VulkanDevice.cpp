#include "VulkanDevice.h"

VulkanDevice::VulkanDevice(VkDevice device)
    : m_vkDevice(device)
{

}

VkDevice VulkanDevice::getVkDevice() const
{
    return m_vkDevice;
}
