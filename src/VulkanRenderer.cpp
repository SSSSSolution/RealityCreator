#include "VulkanApplication.h"
#include "VulkanRenderer.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanDrawable.h"
#include <assert.h>
#include <string.h>
#include "Wrappers.h"
#include "meshdata.h"

VulkanRenderer::VulkanRenderer(VulkanApplication *appObj, VulkanDevice *deviceObj)
    : appObj(appObj), deviceObj(deviceObj)
{
    memset(&depth, 0, sizeof(depth));
//    memset(&connection, 0, sizeof())

    swapChainObj = new VulkanSwapChain(this);
    VulkanDrawable *drawableObj = new VulkanDrawable(this);
    drawableList.push_back(drawableObj);
}

VulkanRenderer::~VulkanRenderer()
{
    delete swapChainObj;
    swapChainObj = nullptr;
}

void VulkanRenderer::initialize()
{
    createPresentationWindow(500, 500);

    swapChainObj->intializeSwapChain();

    createCommandPool();

    // Build the vertex buffer
    createVertexBuffer();

    const bool includeDepth = true;
    // Create the render pass now...
    createRenderPass(includeDepth);

    // Use render pass and create frame buffer
    createFrameBuffer(includeDepth);

    // Create the vertex and fragment shader
    createShaders();

    // Manage the pipeline state objects
    createPipelineStateManagement();
}

void VulkanRenderer::createPresentationWindow(const int &windowWidth, const int &windowHeight)
{
#ifdef _WIN32
#else
    const xcb_setup_t *setup;
    xcb_screen_iterator_t iter;
    int scr;

    connection = xcb_connect(nullptr, &scr);
    if (connection == nullptr) {
        std::cout << "Cannot find a compatible Vulkan ICD.\n";
        exit(-1);
    }

    setup = xcb_get_setup(connection);
    iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0)
        xcb_screen_next(&iter);

    screen = iter.data;
#endif // _WIN32
}

void VulkanRenderer::createCommandPool()
{
    VulkanDevice *deviceObj = appObj->deviceObj;
    VkResult res;

    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.pNext = nullptr;
    cmdPoolInfo.queueFamilyIndex = deviceObj->graphicsQueueWithPresentIndex;

    res = vkCreateCommandPool(deviceObj->device, &cmdPoolInfo, nullptr, &cmdPool);
    assert(res == VK_SUCCESS);
}

void VulkanRenderer::createDepthImage()
{
    VkResult result;
    bool pass;

    VkImageCreateInfo imageInfo = {};

    if (depth.format == VK_FORMAT_UNDEFINED) {
        depth.format = VK_FORMAT_D16_UNORM;
    }

    const VkFormat depthFormat = depth.format;

    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(*deviceObj->gpu, depthFormat, &props);
    if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    } else if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
    } else {
        std::cout << "Unsupported Depth Format , try other Depth format!\n";
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

    // User create image info and create the image objects
    result = vkCreateImage(deviceObj->device, &imageInfo, nullptr, &depth.image);
    assert(result == VK_SUCCESS);

    VkMemoryRequirements memRqrmnt;
    vkGetImageMemoryRequirements(deviceObj->device, depth.image, &memRqrmnt);

    VkMemoryAllocateInfo memAlloc = {};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.pNext = nullptr;
    memAlloc.allocationSize = 0;
    memAlloc.memoryTypeIndex = 0;
    memAlloc.allocationSize = memRqrmnt.size;

    pass = deviceObj->memoryTypeFromProperties(memRqrmnt.memoryTypeBits, 0, &memAlloc.memoryTypeIndex);
    assert(pass);

    result = vkAllocateMemory(deviceObj->device, &memAlloc, nullptr, &depth.mem);
    assert(result == VK_SUCCESS);

    result = vkBindImageMemory(deviceObj->device, depth.image, depth.mem, 0);
    assert(result == VK_SUCCESS);

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
    imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imgViewInfo.flags = 0;

    if (depthFormat == VK_FORMAT_D16_UNORM ||
        depthFormat == VK_FORMAT_D24_UNORM_S8_UINT ||
        depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT)
    {
        imgViewInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    CommandBufferMgr::allocCommandBuffer(&deviceObj->device, cmdPool, &cmdDepthImage);
    CommandBufferMgr::beginCommandBuffer(cmdDepthImage);
    {
        setImageLayout(depth.image,
                       imgViewInfo.subresourceRange.aspectMask,
                       VK_IMAGE_LAYOUT_UNDEFINED,
                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                       (VkAccessFlagBits)0,
                       cmdDepthImage);
    }
    CommandBufferMgr::endCommandBuffer(cmdDepthImage);
    CommandBufferMgr::submitCommandBuffer(deviceObj->queue, &cmdDepthImage);

    imgViewInfo.image = depth.image;
    result = vkCreateImageView(deviceObj->device, &imgViewInfo, nullptr, &depth.view);
    assert(result == VK_SUCCESS);
}

void VulkanRenderer::createRenderPass(bool includeDepth, bool clear)
{
    VkResult result;

    VkAttachmentDescription attachments[2];
    attachments[0].format = swapChainObj->scPublicVars.format;
    attachments[0].samples = NUM_SAMPLES;
    attachments[0].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachments[0].flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;

    if (includeDepth)
    {
        attachments[1].format = depth.format;
        attachments[1].samples = NUM_SAMPLES;
        attachments[1].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments[1].flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
    }

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorReference;
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = includeDepth ? &depthReference : nullptr;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;

    // specify the attachment and subpass associate with render pass
    VkRenderPassCreateInfo rpInfo = {};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpInfo.pNext = nullptr;
    rpInfo.attachmentCount = includeDepth ? 2 : 1;
    rpInfo.pAttachments = attachments;
    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses = &subpass;
    rpInfo.dependencyCount = 0;
    rpInfo.pDependencies = nullptr;

    // create the render pass object
    result = vkCreateRenderPass(deviceObj->device, &rpInfo, nullptr, &renderPass);
    assert(result == VK_SUCCESS);
}

void VulkanRenderer::createFrameBuffer(bool includeDepth)
{
    VkResult result;
    VkImageView attachments[2];
    attachments[1] = depth.view;

    VkFramebufferCreateInfo fbInfo = {};
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.pNext = nullptr;
    fbInfo.renderPass = renderPass;
    fbInfo.attachmentCount = includeDepth ? 2 : 1;
    fbInfo.pAttachments = attachments;
    fbInfo.width = width;
    fbInfo.height = height;
    fbInfo.layers = 1;

    uint32_t i;

    framebuffers.clear();
    framebuffers.resize(swapChainObj->scPublicVars.swapchainImageCount);
    for (i = 0; i < swapChainObj->scPublicVars.swapchainImageCount; i++) {
        attachments[0] = swapChainObj->scPublicVars.colorBuffer[i].view;
        result = vkCreateFramebuffer(deviceObj->device, &fbInfo, nullptr, &framebuffers.at(i));
        assert(result == VK_SUCCESS);
    }
}

void VulkanRenderer::destroyFramebuffers()
{
    for (uint32_t i = 0; i < swapChainObj->scPublicVars.swapchainImageCount; i++) {
        vkDestroyFramebuffer(deviceObj->device, framebuffers.at(i), nullptr);
    }
    framebuffers.clear();
}

void VulkanRenderer::destroyRenderpass()
{
    vkDestroyRenderPass(deviceObj->device, renderPass, nullptr);
}

void VulkanRenderer::destroyDrawableVertexBuffer()
{
    for (auto drawableObject : drawableList)
    {
        drawableObject->destroyVertexBuffer();
    }
}

void VulkanRenderer::destroyDrawableCommandBuffer()
{
    for (auto drawableObject : drawableList)
    {
        drawableObject->destroyCommandBuffer();
    }
}

void VulkanRenderer::destroyDrawableSynchronizationObjects()
{
//    for (auto drawableObject : drawableList)
//    {
//        drawableObject->destroySy
//    }
}

void VulkanRenderer::destroyDepthBuffer()
{
    vkDestroyImageView(deviceObj->device, depth.view, nullptr);
    vkDestroyImage(deviceObj->device, depth.image, nullptr);
    vkFreeMemory(deviceObj->device, depth.mem, nullptr);
}

void VulkanRenderer::destroyCommandBuffer()
{
    VkCommandBuffer cmdBufs[] = { cmdDepthImage, cmdVertexBuffer };
    vkFreeCommandBuffers(deviceObj->device, cmdPool,
                         sizeof(cmdBufs)/sizeof(VkCommandBuffer), cmdBufs);
}

void VulkanRenderer::destroyCommandPool()
{
    VulkanDevice *deviceObj = appObj->deviceObj;
    vkDestroyCommandPool(deviceObj->device, cmdPool, nullptr);
}

void VulkanRenderer::buildSwapChainAndDepthImage()
{
    deviceObj->getDeviceQueue();

    swapChainObj->createSwapChain(cmdDepthImage);

    createDepthImage();
}

void VulkanRenderer::createVertexBuffer()
{
    CommandBufferMgr::allocCommandBuffer(&deviceObj->device, cmdPool, &cmdVertexBuffer);
    CommandBufferMgr::beginCommandBuffer(cmdVertexBuffer);

    for (auto drawableObj : drawableList)
    {
        drawableObj->createVertexBuffer(triangleData, sizeof(triangleData),
                                        sizeof(triangleData[0]), false);
    }

    CommandBufferMgr::endCommandBuffer(cmdVertexBuffer);
    CommandBufferMgr::submitCommandBuffer(deviceObj->queue, &cmdVertexBuffer);
}

void VulkanRenderer::createShaders()
{
    void *vertShaderCode, *fragShaderCode;
    size_t sizeVert, sizeFrag;

    vertShaderCode = readFile("./../Draw-vert.spv", &sizeVert);
    fragShaderCode = readFile("./../Draw-frag.spv", &sizeFrag);

    shaderObj.buildShaderModuleWithSPV((uint32_t *)vertShaderCode, sizeVert,
                                       (uint32_t *)fragShaderCode, sizeFrag);
}

void VulkanRenderer::createPipelineStateManagement()
{
    pipelineObj.createPipelineCache();

    const bool depthPresent = true;
    for (auto drawableObj : drawableList)
    {
        VkPipeline *pipeline = (VkPipeline *)malloc(sizeof(VkPipeline));
        if (pipelineObj.createPipeline(drawableObj, pipeline, &shaderObj, depthPresent))
        {
            pipelineList.push_back(pipeline);
            drawableObj->setPipeline(pipeline);
        } else {
            free(pipeline);
            pipeline = nullptr;
        }
    }
}

void VulkanRenderer::setImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask, const VkCommandBuffer &cmdBuf)
{

}














































