#ifndef VULKANDESCRIPTOR_H
#define VULKANDESCRIPTOR_H

#include "headers.h"

class VulkanDevice;

class VulkanDescriptor
{
public:
    VulkanDescriptor();
    ~VulkanDescriptor();

    void createDescriptor(bool useTexture);

    virtual void createDescriptorSetLayout(bool useTexture) = 0;

    virtual void createDescriptorPool(bool useTexture) = 0;

    virtual void createDescriptorResource() = 0;

    virtual void createDescriptorSet(bool useTexture) = 0;

    virtual void createPipelineLayout() = 0;

public:
    VkPipelineLayout pipelineLayout;

    std::vector<VkDescriptorSetLayout> descLayoutList;

    VkDescriptorPool descriptorPool;

    std::vector<VkDescriptorSet> descriptorSet;

    VulkanDevice *vulkanDevice;
};

#endif // VULKANDESCRIPTOR_H










































