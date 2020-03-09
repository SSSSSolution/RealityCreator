#include "VulkanShader.h"
#include "VulkanDevice.h"
#include "VulkanApplication.h"

VulkanShader::VulkanShader()
{

}

VulkanShader::~VulkanShader()
{

}

void VulkanShader::buildShaderModuleWithSPV(uint32_t *vertShaderText, size_t vertexSPVSize, uint32_t *fragShaderText, size_t fragmentSPVSize)
{
    VulkanDevice *vulkanDevice = &VulkanApplication::getInstance()->vulkanDevice;

    VkResult ret;

    // Fill in the control structure to push the necessary details of the shader
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
    ret = vkCreateShaderModule(vulkanDevice->vkDevice, &moduleCreateInfo, nullptr, &shaderStages[0].module);
    assert(ret == VK_SUCCESS);

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
    ret = vkCreateShaderModule(vulkanDevice->vkDevice, &moduleCreateInfo, nullptr, &shaderStages[1].module);
    assert(ret == VK_SUCCESS);
}


































