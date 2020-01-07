#include <string.h>
#include "VulkanApplication.h"
#include "VulkanDrawable.h"
#include <assert.h>
#include "VulkanRenderer.h"
#include "Wrappers.h"

VulkanDrawable::VulkanDrawable(VulkanRenderer *parent)
{
    memset(&vertexBuffer, 0, sizeof(vertexBuffer));
    rendererObj = parent;

    VkSemaphoreCreateInfo presentCompleteSemaphoreCreateInfo;
    presentCompleteSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    presentCompleteSemaphoreCreateInfo.pNext = nullptr;
    presentCompleteSemaphoreCreateInfo.flags = 0;

    VkSemaphoreCreateInfo drawingCompleteSemaphoreCreateInfo;
    drawingCompleteSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    drawingCompleteSemaphoreCreateInfo.pNext = nullptr;

    VulkanDevice *deviceObj = VulkanApplication::getInstance()->deviceObj;

    vkCreateSemaphore(deviceObj->device, &presentCompleteSemaphoreCreateInfo,
                      nullptr, &presentCompleteSemaphore);
    vkCreateSemaphore(deviceObj->device, &drawingCompleteSemaphoreCreateInfo,
                      nullptr, &drawingCompleteSemaphore);
}

VulkanDrawable::~VulkanDrawable()
{
}

void VulkanDrawable::prepare()
{
    VulkanDevice *device = rendererObj->getVulkanDevice();
    vecCmdDraw.resize(rendererObj->getSwapChain()->scPublicVars.colorBuffer.size());

    for (int i = 0; i < rendererObj->getSwapChain()->scPublicVars.colorBuffer.size(); i++)
    {
//        CommandBufferMgr::allocCommandBuffer(device->getVkDevice(), rendererObj->getCommand )
    }

}
void VulkanDrawable::createVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride, bool useTexture)
{
    VulkanApplication *appObj = VulkanApplication::getInstance();
    VulkanDevice *deviceObj = appObj->deviceObj;

    VkResult result;
    bool pass;

    // create the buffer resource metadata information
    VkBufferCreateInfo bufInfo = {};
    bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufInfo.pNext = nullptr;
    bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufInfo.size = dataSize;
    bufInfo.queueFamilyIndexCount = 0;
    bufInfo.pQueueFamilyIndices = nullptr;
    bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufInfo.flags = 0;

    // Create the buffer resource
    result = vkCreateBuffer(deviceObj->device, &bufInfo,
                            nullptr, &vertexBuffer.buf);
    assert(result == VK_SUCCESS);

    VkMemoryRequirements memRqrmnt;
    vkGetBufferMemoryRequirements(deviceObj->device, vertexBuffer.buf, &memRqrmnt);

    // create memory allocation metadata information
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.memoryTypeIndex = 0;
    allocInfo.allocationSize = memRqrmnt.size;

    // Get the compatible type of memory
    pass = deviceObj->memoryTypeFromProperties(memRqrmnt.memoryTypeBits,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                               &allocInfo.memoryTypeIndex);
    assert(pass);

    // Allocate the physical backing for buffer source
    result = vkAllocateMemory(deviceObj->device, &allocInfo, nullptr, &(vertexBuffer.mem));
    assert(result == VK_SUCCESS);
    vertexBuffer.bufferInfo.range = memRqrmnt.size;
    vertexBuffer.bufferInfo.offset = 0;

    // Map the physical device memory region to the host
    uint8_t *pData;
    result = vkMapMemory(deviceObj->device, vertexBuffer.mem, 0, memRqrmnt.size,
                         0, (void **)&pData);

    // Copy the data in the mapped memory
    memcpy(pData, vertexData, dataSize);

    // Unmap the device memory
    vkUnmapMemory(deviceObj->device, vertexBuffer.mem);

    // Bind the allocated buffer resource to the device memory
    result = vkBindBufferMemory(deviceObj->device, vertexBuffer.buf,
                                vertexBuffer.mem, 0);
    assert(result == VK_SUCCESS);

    viIpBind.binding = 0;
    viIpBind.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    viIpBind.stride = dataStride;

    viIpAttrb[0].binding = 0;
}

void VulkanDrawable::destroyVertexBuffer()
{
    VulkanDevice *device = rendererObj->getVulkanDevice();

    vkDestroyBuffer(device->getVkDevice(), vertexBuffer.buf, nullptr);
    vkFreeMemory(device->getVkDevice(), vertexBuffer.mem, nullptr);
}






























