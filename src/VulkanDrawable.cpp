#include "VulkanDrawable.h"
#include "VulkanApplication.h"
#include "VulkanDevice.h"
#include "VulkanRenderer.h"
#include "VulkanSwapChain.h"
#include "CommandBufferMgr.h"

VulkanDrawable::VulkanDrawable(VulkanRenderer *parent)
{
    vulkanRenderer = parent;

    VkSemaphoreCreateInfo presentCompleteSemaphoreCreateInfo;
    presentCompleteSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    presentCompleteSemaphoreCreateInfo.pNext = nullptr;
    presentCompleteSemaphoreCreateInfo.flags = 0;

    VkSemaphoreCreateInfo drawingSemaphoreCreateInfo;
    drawingSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    drawingSemaphoreCreateInfo.pNext = nullptr;
    drawingSemaphoreCreateInfo.flags = 0;

    VulkanDevice *vulkanDevice = &VulkanApplication::getInstance()->vulkanDevice;

    vkCreateSemaphore(vulkanDevice->vkDevice, &presentCompleteSemaphoreCreateInfo, nullptr, &presentCompleteSemaphore);
    vkCreateSemaphore(vulkanDevice->vkDevice, &drawingSemaphoreCreateInfo, nullptr, &drawingCompleteSemaphore);

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = 0;

    VkResult ret = vkCreateFence(VulkanApplication::getInstance()->vulkanDevice.vkDevice, &fenceInfo, nullptr, &vkFence);
    assert(ret == VK_SUCCESS);

    prepared = false;
}

VulkanDrawable::~VulkanDrawable()
{

}

void VulkanDrawable::createVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture)
{
    VulkanApplication *vulkanApplication = VulkanApplication::getInstance();
    VulkanDevice *vulkanDevice = &vulkanApplication->vulkanDevice;

    VkResult ret;
    bool pass;

    // Create the Buffer resource metadata information
    VkBufferCreateInfo bufInfo = {};
    bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufInfo.pNext = nullptr;
    bufInfo.flags = 0;
    bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufInfo.size = dataSize;
    bufInfo.queueFamilyIndexCount = 0;
    bufInfo.pQueueFamilyIndices = nullptr;
    bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // Create the Buffer resource
    ret = vkCreateBuffer(vulkanDevice->vkDevice, &bufInfo, nullptr, &VertexBuffer.buf);
    assert(ret == VK_SUCCESS);

    // Get the Buffer  resource requirements
    VkMemoryRequirements memRqrmnt;
    vkGetBufferMemoryRequirements(vulkanDevice->vkDevice, VertexBuffer.buf, &memRqrmnt);

    // Create memory allocation metadata information
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.memoryTypeIndex = 0;
    allocInfo.allocationSize = memRqrmnt.size;
    qDebug() << "allocate size:" << memRqrmnt.size;
    // Get the compatible type of memory
    pass = vulkanDevice->memoryTypeFromProperties(memRqrmnt.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &allocInfo.memoryTypeIndex);
    assert(pass);

    // Allocate the physical device memory region to the host
    ret = vkAllocateMemory(vulkanDevice->vkDevice, &allocInfo, nullptr, &(VertexBuffer.mem));
    assert(ret == VK_SUCCESS);
    VertexBuffer.bufferInfo.range = memRqrmnt.size;
    qDebug() << "memRqmnt: " << memRqrmnt.size;
    VertexBuffer.bufferInfo.offset = 0;

    // Map the physical device memory region to the host
    uint8_t *pData;
    ret = vkMapMemory(vulkanDevice->vkDevice, VertexBuffer.mem, 0, memRqrmnt.size, 0, (void **)&pData);
    assert(ret == VK_SUCCESS);

    // Copy the data in the mapped memory
    memcpy(pData, vertexData, dataSize);
    qDebug() << "data size: " << dataSize;

    // Unmap the device memory
    vkUnmapMemory(vulkanDevice->vkDevice, VertexBuffer.mem);

    // Bind the allocated buffer resource to the device memory
    ret = vkBindBufferMemory(vulkanDevice->vkDevice, VertexBuffer.buf, VertexBuffer.mem, 0);
    assert(ret == VK_SUCCESS);

    viIpBind.binding = 0;
    viIpBind.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    viIpBind.stride = dataStride;

    viIpAttrb[0].binding = 0;
    viIpAttrb[0].location = 0;
    viIpAttrb[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    viIpAttrb[0].offset = 0;
    viIpAttrb[1].binding = 0;
    viIpAttrb[1].location = 1;
    viIpAttrb[1].format = useTexture ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
    viIpAttrb[1].offset = 16;
}


void VulkanDrawable::prepare()
{
    qDebug()<< __func__ << vulkanRenderer->vulkanSwapChain->colorBufferList.size();
    VulkanDevice *vulkanDevice = vulkanRenderer->vulkanDevice;
    vecCmdDraw.resize(vulkanRenderer->vulkanSwapChain->colorBufferList.size());

    for (uint32_t i = 0; i < vulkanRenderer->vulkanSwapChain->colorBufferList.size(); i++) {
        CommandBufferMgr::allocCommandBuffer(&vulkanDevice->vkDevice, vulkanRenderer->cmdPool, &vecCmdDraw[i]);
        CommandBufferMgr::beginCommandBuffer(vecCmdDraw[i]);

        // Create the render pass instance
        recordCommandBuffer(i, &vecCmdDraw[i]);

        CommandBufferMgr::endCommandBuffer(vecCmdDraw[i]);
    }
    prepared = true;
}

void VulkanDrawable::recordCommandBuffer(int currentImage, VkCommandBuffer *cmdDraw)
{
    qDebug() << __func__ << currentImage;
    VulkanDevice *vulkanDevice = vulkanRenderer->vulkanDevice;

    VkClearValue clearValues[2];
    clearValues[0].color.float32[0] = 0.0f;
    clearValues[0].color.float32[1] = 0.0f;
    clearValues[0].color.float32[2] = 0.0f;
    clearValues[0].color.float32[3] = 0.0f;

    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;

    VkRenderPassBeginInfo renderPassBegin = {};
    renderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBegin.pNext = nullptr;
    renderPassBegin.renderPass = vulkanRenderer->renderPass;
    renderPassBegin.framebuffer = vulkanRenderer->framebuffers[currentImage];
    renderPassBegin.renderArea.offset.x = 0;
    renderPassBegin.renderArea.offset.y = 0;
    renderPassBegin.renderArea.extent.width = vulkanRenderer->width;
    renderPassBegin.renderArea.extent.height = vulkanRenderer->height;
    renderPassBegin.clearValueCount = 2;
    renderPassBegin.pClearValues = clearValues;

    // Start recording the render pass instance
    vkCmdBeginRenderPass(*cmdDraw, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);

    // Bound the command buffer with the graphics pipeline
    vkCmdBindPipeline(*cmdDraw, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);

    vkCmdBindDescriptorSets(*cmdDraw, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout, 0, 1, descriptorSet.data(), 0, nullptr);
    // Bound the command buffer with the graphics pipeline
    const VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(*cmdDraw, 0, 1, &VertexBuffer.buf, offsets);

    // Define the dynamic viweport here
    initViewports(cmdDraw);

    initScissors(cmdDraw);

    vkCmdDraw(*cmdDraw, 3*2*6, 1, 0, 0);

    vkCmdEndRenderPass(*cmdDraw);
}

void VulkanDrawable::render()
{
    qDebug() << __func__;
    if (!prepared)
        return;
    VkResult ret;

    VulkanDevice *vulkanDevice = vulkanRenderer->vulkanDevice;
    VulkanSwapChain *vulkanSwapChain = vulkanRenderer->vulkanSwapChain;

    uint32_t &currentColorImage = vulkanSwapChain->currentColorBuffer;
    qDebug() << currentColorImage;
    VkSwapchainKHR &swapChain = vulkanSwapChain->swapChain;

//    VkFence nullFence = VK_NULL_HANDLE;
    vkResetFences(VulkanApplication::getInstance()->vulkanDevice.vkDevice, 1, &vkFence);
    ret = vulkanSwapChain->vkAcquireNextImageKHR(vulkanDevice->vkDevice, swapChain, UINT64_MAX,
                                                 presentCompleteSemaphore, vkFence, &currentColorImage);
    vkWaitForFences(VulkanApplication::getInstance()->vulkanDevice.vkDevice, 1, &vkFence, VK_TRUE, UINT_MAX);
    qDebug() << "next image index: " << currentColorImage;
    VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &presentCompleteSemaphore;
    submitInfo.pWaitDstStageMask = &submitPipelineStages;
    submitInfo.commandBufferCount = (uint32_t)sizeof(&vecCmdDraw[currentColorImage])/sizeof(VkCommandBuffer);
    submitInfo.pCommandBuffers = &vecCmdDraw[currentColorImage];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &drawingCompleteSemaphore;

    // Queue the command buffer for execution
    CommandBufferMgr::submitCommandBuffer(vulkanDevice->queue, &vecCmdDraw[currentColorImage], &submitInfo);

    //present the image in the window
    VkPresentInfoKHR present = {};
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = nullptr;
    present.swapchainCount = 1;
    present.pSwapchains = &swapChain;
    present.pImageIndices = &currentColorImage;
    present.pWaitSemaphores = &drawingCompleteSemaphore;
    present.waitSemaphoreCount = 1;
    present.pResults = nullptr;

    // Queue the image for presentation
    ret = vulkanSwapChain->vkQueuePresentKHR(vulkanDevice->queue, &present);
    assert(ret == VK_SUCCESS);

}

void VulkanDrawable::update()
{
    if (!prepared)
        return;

    VulkanDevice *vulkanDevice = vulkanRenderer->vulkanDevice;
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

    glm::mat4 View = glm::lookAt( glm::vec3(0, 0, 5),
                                  glm::vec3(0, 0, 0),
                                  glm::vec3(0, 1, 0) );
    glm::mat4 Model = glm::mat4(1.0f);
    static float rot = 0;
    rot += 0.0005f;
    Model = glm::rotate(Model, rot, glm::vec3(0.0f, 1.0f, 0.0f))
          * glm::rotate(Model, rot, glm::vec3(1.0f, 1.0f, 1.0f));

    glm::mat4 MVP = Projection * View * Model;


    VkResult ret = vkInvalidateMappedMemoryRanges(vulkanDevice->vkDevice, 1, &UniformData.mappedRange[0]);
    assert(ret == VK_SUCCESS);

    memcpy(UniformData.pData, &MVP, sizeof(MVP));
    ret = vkFlushMappedMemoryRanges(vulkanDevice->vkDevice, 1, &UniformData.mappedRange[0]);
    assert(ret == VK_SUCCESS);
}

#define NUMBER_OF_VIEWPORTS 1
#define NUMBER_OF_SCISSORS NUMBER_OF_VIEWPORTS

void VulkanDrawable::initViewports(VkCommandBuffer *cmd)
{
    viewport.height = (float)vulkanRenderer->height;
    viewport.width = (float)vulkanRenderer->width;
    viewport.minDepth = (float)0.0f;
    viewport.maxDepth = (float)1.0f;
    viewport.x = 0;
    viewport.y = 0;
    vkCmdSetViewport(*cmd, 0, NUMBER_OF_VIEWPORTS, &viewport);
}

void VulkanDrawable::initScissors(VkCommandBuffer *cmd)
{
    scissor.extent.width = vulkanRenderer->width;
    scissor.extent.height = vulkanRenderer->height;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    vkCmdSetScissor(*cmd, 0, NUMBER_OF_SCISSORS, &scissor);
}

void VulkanDrawable::createDescriptorSetLayout(bool useTexture)
{
    VkDescriptorSetLayoutBinding layoutBindings[2];
    layoutBindings[0].binding = 0;
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBindings[0].descriptorCount = 1;
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBindings[0].pImmutableSamplers = nullptr;

    if (useTexture) {
        layoutBindings[1].binding = 1;
        layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        layoutBindings[1].descriptorCount = 1;
        layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        layoutBindings[1].pImmutableSamplers = nullptr;
    }

    VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
    descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext = nullptr;
    descriptorLayout.bindingCount = useTexture ? 2 : 1;
    descriptorLayout.pBindings = layoutBindings;

    VkResult ret;
    descLayoutList.resize(1);
    ret = vkCreateDescriptorSetLayout(vulkanDevice->vkDevice, &descriptorLayout, nullptr, descLayoutList.data());
    assert(ret == VK_SUCCESS);
}

void VulkanDrawable::createPipelineLayout()
{
    // Setup the push constant range
    const unsigned pushConstantRangeCount = 1;
    VkPushConstantRange pushConstantRanges[pushConstantRangeCount] = {};
    pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRanges[0].offset = 0;
    pushConstantRanges[0].size = 8;



    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantRangeCount;
    pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges;
    pipelineLayoutCreateInfo.setLayoutCount = (uint32_t)descLayoutList.size();
    pipelineLayoutCreateInfo.pSetLayouts = descLayoutList.data();

    VkResult ret;
    ret = vkCreatePipelineLayout(vulkanDevice->vkDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
    assert(ret == VK_SUCCESS);
}

void VulkanDrawable::createDescriptorPool(bool useTexture)
{
    VkResult ret;

    std::vector<VkDescriptorPoolSize> descriptorTypePool;

    descriptorTypePool.push_back(VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 });

    if (useTexture) {
        descriptorTypePool.push_back(VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 });
    }

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.maxSets = 1;
    descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolCreateInfo.poolSizeCount = (uint32_t)descriptorTypePool.size();
    descriptorPoolCreateInfo.pPoolSizes = descriptorTypePool.data();

    ret = vkCreateDescriptorPool(vulkanDevice->vkDevice, &descriptorPoolCreateInfo, nullptr, &descriptorPool);
    assert(ret == VK_SUCCESS);
}

void VulkanDrawable::createDescriptorResource()
{
    createUniformBuffer();
}

void VulkanDrawable::createDescriptorSet(bool useTexture)
{
    VulkanPipeline *vulkanPipeline = &vulkanRenderer->vulkanPipeline;
    VkResult ret;

    VkDescriptorSetAllocateInfo dsAllocInfo[1];
    dsAllocInfo[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    dsAllocInfo[0].pNext = nullptr;
    dsAllocInfo[0].descriptorPool = descriptorPool;
    dsAllocInfo[0].descriptorSetCount = 1;
    dsAllocInfo[0].pSetLayouts = descLayoutList.data();

    descriptorSet.resize(1);

    ret = vkAllocateDescriptorSets(vulkanDevice->vkDevice, dsAllocInfo, descriptorSet.data());
    assert(ret == VK_SUCCESS);

    VkWriteDescriptorSet writes[2];
    memset(&writes, 0, sizeof(writes));

    writes[0] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = nullptr;
    writes[0].dstSet = descriptorSet[0];
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &UniformData.bufferInfo;
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;

    if (useTexture) {
        writes[1] = {};
        writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[1].dstSet= descriptorSet[0];
        writes[1].dstBinding = 1;
        writes[1].descriptorCount = 1;
        writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[1].pImageInfo = nullptr;
        writes[1].dstArrayElement = 0;
    }

    vkUpdateDescriptorSets(vulkanDevice->vkDevice, useTexture ? 2 : 1, writes, 0, nullptr);
}

void VulkanDrawable::createUniformBuffer()
{
    VkResult ret;
    bool pass;

    Projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    View = glm::lookAt(glm::vec3(10, 3, 10),
                       glm::vec3(0, 0, 0),
                       glm::vec3(0, -1, 0));
    Model = glm::mat4(1.0f);
    MVP = Projection * View * Model;

    VkBufferCreateInfo bufInfo = {};
    bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufInfo.pNext = nullptr;
    bufInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufInfo.flags = 0;
    bufInfo.size = sizeof(MVP);
    bufInfo.queueFamilyIndexCount = 0;
    bufInfo.pQueueFamilyIndices = nullptr;
    bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufInfo.flags = 0;

    ret = vkCreateBuffer(vulkanDevice->vkDevice, &bufInfo, nullptr, &UniformData.buffer);
    assert(ret == VK_SUCCESS);

    VkMemoryRequirements memRqrmnt;
    vkGetBufferMemoryRequirements(vulkanDevice->vkDevice, UniformData.buffer, &memRqrmnt);

    VkMemoryAllocateInfo memAllocInfo = {};
    memAllocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.pNext				= NULL;
    memAllocInfo.memoryTypeIndex		= 0;
    memAllocInfo.allocationSize = memRqrmnt.size;

    // Determine the type of memory required
    // with the help of memory properties
    pass = vulkanDevice->memoryTypeFromProperties(memRqrmnt.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAllocInfo.memoryTypeIndex);
    assert(pass);

    // Allocate the memory for buffer objects
    ret = vkAllocateMemory(vulkanDevice->vkDevice, &memAllocInfo, NULL, &(UniformData.memory));
    assert(ret == VK_SUCCESS);

    // Map the GPU memory on to local host
    ret = vkMapMemory(vulkanDevice->vkDevice, UniformData.memory, 0, memRqrmnt.size, 0, (void **)&UniformData.pData);
    assert(ret == VK_SUCCESS);

    // Copy computed data in the mapped buffer
    memcpy(UniformData.pData, &MVP, sizeof(MVP));

    // We have only one Uniform buffer object to update
    UniformData.mappedRange.resize(1);

    // Populate the VkMappedMemoryRange data structure
    UniformData.mappedRange[0].sType	= VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    UniformData.mappedRange[0].memory	= UniformData.memory;
    UniformData.mappedRange[0].offset	= 0;
    UniformData.mappedRange[0].size		= sizeof(MVP);

    // Invalidate the range of mapped buffer in order to make it visible to the host.
    // If the memory property is set with VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    // then the driver may take care of this, otherwise for non-coherent
    // mapped memory vkInvalidateMappedMemoryRanges() needs to be called explicitly.
    vkInvalidateMappedMemoryRanges(vulkanDevice->vkDevice, 1, &UniformData.mappedRange[0]);

    // Bind the buffer device memory
    ret = vkBindBufferMemory(vulkanDevice->vkDevice,	UniformData.buffer, UniformData.memory, 0);
    assert(ret == VK_SUCCESS);

    // Update the local data structure with uniform buffer for house keeping
    UniformData.bufferInfo.buffer	= UniformData.buffer;
    UniformData.bufferInfo.offset	= 0;
    UniformData.bufferInfo.range	= sizeof(MVP);
    UniformData.memRqrmnt			= memRqrmnt;
}



































