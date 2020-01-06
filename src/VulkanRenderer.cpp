#include "VulkanRenderer.h"
#include "VulkanDevice.h"
#include <assert.h>
#include <string.h>
#include "Wrappers.h"

VulkanRenderer::VulkanRenderer(VulkanDevice *device)
    : m_device(device)
{
    assert(m_device != nullptr);

    memset(&Depth, 0, sizeof(Depth));
}

void VulkanRenderer::initialize()
{
    createCommandPool();
    createRenderPass(true);

    createCommandPool();

    buildSwapChainAndDepthImage();
}

void VulkanRenderer::createRenderPass(bool includeDepth, bool clear)
{
    VkResult ret;

    VkAttachmentDescription attachments[2];
//    attachments[0].format =
}

void VulkanRenderer::createCommandPool()
{
    VkResult res;

    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.pNext = nullptr;
    cmdPoolInfo.queueFamilyIndex = 0;
    cmdPoolInfo.flags = 0;

    res = vkCreateCommandPool(m_device->getVkDevice(), &cmdPoolInfo,
                              nullptr, &m_cmdPool);
    assert(res == VK_SUCCESS);
}


void VulkanRenderer::buildSwapChainAndDepthImage()
{
    // Get the appropriate queue to submit the command into
    m_device->getDeviceQueue();

    createDepthImage();
}

void VulkanRenderer::createDepthImage()
{
    VkResult res;
    bool pass;

    VkImageCreateInfo imageInfo = {};

    if (Depth.format == VK_FORMAT_UNDEFINED) {
        Depth.format = VK_FORMAT_D16_UNORM;
    }

    const VkFormat depthFormat = Depth.format;

    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(m_device->getPhysicalDevice(),
                                        depthFormat, &props);
    if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    } else if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
    }

    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = nullptr;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = depthFormat;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = NUM_SAMPLES;
    imageInfo.queueFamilyIndexCount = 0;
    imageInfo.pQueueFamilyIndices = nullptr;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.flags = 0;

    res = vkCreateImage(m_device->getVkDevice(), &imageInfo, nullptr, &Depth.image);
    assert(res == VK_SUCCESS);

    VkMemoryRequirements memRqrmnt;
    vkGetImageMemoryRequirements(m_device->getVkDevice(), Depth.image, &memRqrmnt);

    VkMemoryAllocateInfo memAlloc = {};
    memAlloc.pNext = nullptr;
    memAlloc.allocationSize = 0;
    memAlloc.memoryTypeIndex = 0;
    memAlloc.allocationSize = memRqrmnt.size;

    // Determine the type of memory required with the helo of memory properties
    pass = m_device->memoryTypeFromProperties(memRqrmnt.memoryTypeBits,
                                              0, &memAlloc.memoryTypeIndex);
    assert(pass);

    // Allocate the memory for image objects
    res = vkBindImageMemory(m_device->getVkDevice(), Depth.image, Depth.mem, 0);
    assert(res == VK_SUCCESS);

    VkImageViewCreateInfo imgViewInfo = {};
    imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imgViewInfo.pNext = nullptr;
    imgViewInfo.image = VK_NULL_HANDLE;
    imgViewInfo.format = depthFormat;
    imgViewInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY };
    imgViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    imgViewInfo.subresourceRange.baseMipLevel = 0;
    imgViewInfo.subresourceRange.levelCount = 1;
    imgViewInfo.subresourceRange.baseArrayLayer = 0;
    imgViewInfo.subresourceRange.layerCount = 1;
    imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imgViewInfo.flags = 0;

    if (depthFormat == VK_FORMAT_D16_UNORM_S8_UINT ||
        depthFormat == VK_FORMAT_D24_UNORM_S8_UINT ||
        depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT) {
        imgViewInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    // use command buffer to create the depth image. This includes
    // Command buffer allocation, recording with begin/end scope and submission
    CommandBufferMgr::allocCommandBuffer((const VkDevice *)m_device->getVkDevice(),
                                         m_cmdPool, &cmdDepthImage);

    CommandBufferMgr::beginCommandBuffer(cmdDepthImage);
    {
        setImageLayout(Depth.image,
                       imgViewInfo.subresourceRange.aspectMask,
                       VK_IMAGE_LAYOUT_UNDEFINED,
                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                       (VkAccessFlagBits)0,
                       cmdDepthImage);
    }
    CommandBufferMgr::endCommandBuffer(cmdDepthImage);
    CommandBufferMgr::submitCommandBuffer(m_device->queue, &cmdDepthImage);

    imgViewInfo.image = Depth.image;
    res = vkCreateImageView(m_device->getVkDevice(),
                            &imgViewInfo, nullptr, &Depth.view);
    assert(res == VK_SUCCESS);
}

void VulkanRenderer::setImageLayout(VkImage image,
                                    VkImageAspectFlags aspectMask,
                                    VkImageLayout oldImageLayout,
                                    VkImageLayout newImageLayout,
                                    VkAccessFlagBits srcAccessMask,
                                    const VkCommandBuffer &cmdBuf)
{
    assert(cmdBuf != VK_NULL_HANDLE);

    assert(m_device->queue != VK_NULL_HANDLE);

    VkImageMemoryBarrier imgMemoryBarrier = {};
    imgMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imgMemoryBarrier.pNext = nullptr;
    imgMemoryBarrier.srcAccessMask = srcAccessMask;
    imgMemoryBarrier.dstAccessMask = 0;
    imgMemoryBarrier.oldLayout = oldImageLayout;
    imgMemoryBarrier.newLayout = newImageLayout;
    imgMemoryBarrier.image = image;
    imgMemoryBarrier.subresourceRange.aspectMask = aspectMask;
    imgMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imgMemoryBarrier.subresourceRange.levelCount = 1;
    imgMemoryBarrier.subresourceRange.layerCount = 1;

    if (oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        imgMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    switch (newImageLayout)
    {
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        imgMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        imgMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imgMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;

    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        imgMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        break;

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        imgMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;
    }

    VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    vkCmdPipelineBarrier(cmdBuf, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &imgMemoryBarrier);
}

void VulkanRenderer::createVertexBuffer()
{
    VkDevice vkDevice = m_device->getVkDevice();
    CommandBufferMgr::allocCommandBuffer(&vkDevice,
                                         cmdPool, &cmdVertexBuffer);
    CommandBufferMgr::beginCommandBuffer(cmdVertexBuffer);

//    for (auto )

    CommandBufferMgr::endCommandBuffer(cmdVertexBuffer);
    CommandBufferMgr::submitCommandBuffer(m_device->queue, &cmdVertexBuffer);

}

VulkanDevice *VulkanRenderer::getVulkanDevice()
{
    return m_device;
}






















