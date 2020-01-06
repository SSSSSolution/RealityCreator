#include "VulkanDevice.h"
#include "VulkanPhysicalDevice.h"
VulkanDevice::VulkanDevice(VkDevice device, VulkanPhysicalDevice *physicalDevic)
    : m_vkDevice(device), m_physicalDevice(physicalDevic)
{

}

VkDevice VulkanDevice::getVkDevice() const
{
    return m_vkDevice;
}

VkPhysicalDevice VulkanDevice::getPhysicalDevice() const
{
    return m_physicalDevice->getVkPhysicalDevice();
}

bool VulkanDevice::memoryTypeFromProperties(uint32_t typeBits, VkFlags requirementsMask, uint32_t *typeIndex)
{
    VkPhysicalDeviceMemoryProperties memoryProperties = m_physicalDevice->getMemoryProperties();

    for (uint32_t i = 0; i < 32; ++i) {
        if ((typeBits & 1) == 1) {
            if ((memoryProperties.memoryTypes[i].propertyFlags &
                 requirementsMask) == requirementsMask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    return false;
}

void VulkanDevice::getDeviceQueue()
{
    // Parminder: this depends on intialiing the SwapChain to
    // get the graphics queue with presentation support

    vkGetDeviceQueue(m_vkDevice, graphicsQueueWithPresentIndex, 0, &queue);

}
























