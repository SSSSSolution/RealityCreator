#ifndef VULKANPIPELINE_H
#define VULKANPIPELINE_H

#include "headers.h"
class VulkanShader;
class VulkanDrawable;
class VulkanDevice;
class VulkanApplication;

#define NUMBER_OF_VIEWPORT 1
#define NUMBER_OF_SCISSORS NUMBER_OF_VIEWPORT
class VulkanPipeline
{
public:
    VulkanPipeline();
    ~VulkanPipeline();

    void createPipelineCache();

    bool createPipeline(VulkanDrawable *drawableObj, VkPipeline *pipeline,
                        VulkanShader *shaderObj, VkBool32 includeDepth, VkBool32 includeVi = true);
    void destroyPipelineCache();
public:
    VkPipelineCache pipelineCache;
    VkPipelineLayout pipelineLayout;
    VulkanApplication *appObj;
    VulkanDevice *deviceObj;
};

#endif // VULKANPIPELINE_H
