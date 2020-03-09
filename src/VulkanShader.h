#ifndef VULKANSHADER_H
#define VULKANSHADER_H

#include "headers.h"

class VulkanShader
{
public:
    VulkanShader();
    ~VulkanShader();

    void buildShaderModuleWithSPV(uint32_t *vertShaderText, size_t vertexSPVSize,
                                  uint32_t *fragShaderText, size_t fragmentSPVSize);

public:
    VkPipelineShaderStageCreateInfo shaderStages[2];
};

#endif // VULKANSHADER_H
