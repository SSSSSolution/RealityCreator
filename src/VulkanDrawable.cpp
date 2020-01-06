#include <string.h>
#include "VulkanDrawable.h"
#include <assert.h>
#include "VulkanRenderer.h"
#include "Wrappers.h"

VulkanDrawable::VulkanDrawable(VulkanRenderer *parent)
{
    memset(&vertexBuffer, 0, sizeof(vertexBuffer));
    rendererObj = parent;
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
    VkResult ret;
    bool pass;

    VkBufferCreateInfo bufInfo = {};
    bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufInfo.pNext = nullptr;
    bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufInfo.size = dataSize;
    bufInfo.queueFamilyIndexCount = 0;
    bufInfo.pQueueFamilyIndices = nullptr;
    bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufInfo.flags = 0;

    ret = vkCreateBuffer(rendererObj->getVulkanDevice()->getVkDevice(),
                         &bufInfo, nullptr, &vertexBuffer.buf);
    assert(ret == VK_SUCCESS);

    VkMemoryRequirements memRqrmnt;
    vkGetBufferMemoryRequirements(rendererObj->getVulkanDevice()->getVkDevice(),
                                  vertexBuffer.buf, &memRqrmnt);

    // Create memory allocation matadata information
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.memoryTypeIndex = 0;
    allocInfo.allocationSize = memRqrmnt.size;

    // Get the compatible type of memory
    pass = rendererObj->getVulkanDevice()->memoryTypeFromProperties(memRqrmnt.memoryTypeBits,
                                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                                    &allocInfo.memoryTypeIndex);
    assert(pass);

    // Allocate the physical backing for buffer resource
    ret = vkAllocateMemory(rendererObj->getVulkanDevice()->getVkDevice(),
                           &allocInfo, nullptr,&(vertexBuffer.mem));
    assert(ret == VK_SUCCESS);
    vertexBuffer.bufferInfo.range = memRqrmnt.size;
    vertexBuffer.bufferInfo.offset = 0;

    // Map the physical device memory region to the host
    uint8_t *pData;
    ret = vkMapMemory(rendererObj->getVulkanDevice()->getVkDevice(),
                      vertexBuffer.mem, 0, memRqrmnt.size, 0,
                      (void **)&pData);
    assert(ret == VK_SUCCESS);

    // Copy the data in the mapped memory
    memcpy(pData, vertexData, dataSize);

    vkUnmapMemory(rendererObj->getVulkanDevice()->getVkDevice(),
                  vertexBuffer.mem);
    // Bind the allocated buffer resource to the device memory
    ret = vkBindBufferMemory(rendererObj->getVulkanDevice()->getVkDevice(),
                             vertexBuffer.buf, vertexBuffer.mem, 0);
    assert(ret == VK_SUCCESS);

    viIpBind.binding = 0;
    viIpBind.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    viIpBind.stride = dataStride;

    viIpAttrb[0].binding = 0;
    viIpAttrb[0].location = 0;
    viIpAttrb[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    viIpAttrb[0].offset = 0;
    viIpAttrb[1].binding = 1;
    viIpAttrb[1].location = 1;
    viIpAttrb[1].format = useTexture ? VK_FORMAT_R32G32_SFLOAT :
                                       VK_FORMAT_R32G32B32A32_SFLOAT;
    viIpAttrb[1].offset = 16;
}

void VulkanDrawable::destroyVertexBuffer()
{
    VulkanDevice *device = rendererObj->getVulkanDevice();

    vkDestroyBuffer(device->getVkDevice(), vertexBuffer.buf, nullptr);
    vkFreeMemory(device->getVkDevice(), vertexBuffer.mem, nullptr);
}






























