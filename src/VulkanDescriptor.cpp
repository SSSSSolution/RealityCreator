#include "VulkanDescriptor.h"
#include "VulkanDevice.h"
#include "VulkanApplication.h"

VulkanDescriptor::VulkanDescriptor()
{
    vulkanDevice = &VulkanApplication::getInstance()->vulkanDevice;
}

VulkanDescriptor::~VulkanDescriptor()
{
}

void VulkanDescriptor::createDescriptor(bool useTexture)
{
    createDescriptorResource();

    createDescriptorPool(useTexture);

    createDescriptorSet(useTexture);
}
