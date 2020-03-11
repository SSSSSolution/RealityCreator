#ifndef VULKANPIPELINE_H
#define VULKANPIPELINE_H

#include "headers.h"
#include "VulkanDevice.h"
#include "VulkanApplication.h"

#define NUMBER_OF_VIEWPORTS 1
#define NUMBER_OF_SCISSORS NUMBER_OF_VIEWPORTS

class VulkanDrawable;
class VulkanShader;
class VulkanPipeline
{
public:
    VulkanPipeline();
    ~VulkanPipeline();

    void createPipelineCache();
    bool createPipeline(VulkanDrawable *vulkanDrawable, VkPipeline *pipeline, VulkanShader *vulkanShader,
                        VkBool32 includeDepth, VkBool32 includeVi = VK_TRUE);

private:
    VulkanApplication *vulkanApplication;
    VulkanDevice *vulkanDevice;
    VkPipelineCache pipelineCache;
    VkPipelineLayout pipelineLayout;
};

#endif // VULKANPIPELINE_H
