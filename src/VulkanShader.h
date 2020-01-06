#ifndef VULKANSHADER_H
#define VULKANSHADER_H

#include <vulkan/vulkan.h>

class VulkanShader
{
public:
    VulkanShader() {}

    ~VulkanShader() {}

    void buildShaderModuleWithSPV(uint32_t *vertShaderText,
                                  size_t vertexSPVSize,
                                  uint32_t *fragShaderText,
                                  size_t fragmentSPVSize);

    void destroyShaders();

    VkPipelineShaderStageCreateInfo shaderStages[2];
};

#endif // VULKANSHADER_H
