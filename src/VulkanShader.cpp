#include "VulkanShader.h"
#include "VulkanDevice.h"
#include "VulkanApplication.h"
#include <assert.h>

void VulkanShader::buildShaderModuleWithSPV(uint32_t *vertShaderText, size_t vertexSPVSize, uint32_t *fragShaderText, size_t fragmentSPVSize)
{
    VulkanDevice *deviceObj = VulkanApplication::getInstance()->deviceObj;
    VkResult result;

    // Fill in the control structure to push the necessary
    // Details of the shader
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].pNext = nullptr;
    shaderStages[0].pSpecializationInfo = nullptr;
    shaderStages[0].flags = 0;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].pName = "main";

    VkShaderModuleCreateInfo moduleCreateInfo;
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = nullptr;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = vertexSPVSize;
    moduleCreateInfo.pCode = vertShaderText;

    result = vkCreateShaderModule(deviceObj->device, &moduleCreateInfo,
                                  nullptr, &shaderStages[0].module);
    assert(result == VK_SUCCESS);

    std::vector<unsigned int> fragSPV;
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].pNext = nullptr;
    shaderStages[1].pSpecializationInfo = nullptr;
    shaderStages[1].flags = 0;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].pName = "main";

    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = nullptr;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = fragmentSPVSize;
    moduleCreateInfo.pCode = fragShaderText;

    result = vkCreateShaderModule(deviceObj->device, &moduleCreateInfo,
                                  nullptr, &shaderStages[1].module);
    assert(result == VK_SUCCESS);
}

void VulkanShader::destroyShaders()
{
    VulkanDevice *deviceObj = VulkanApplication::getInstance()->deviceObj;
    vkDestroyShaderModule(deviceObj->device, shaderStages[0].module, nullptr);
    vkDestroyShaderModule(deviceObj->device, shaderStages[1].module, nullptr);
}






















