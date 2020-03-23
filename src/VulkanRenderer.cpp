#include "VulkanRenderer.h"
#include "VulkanSwapChain.h"
#include "VulkanApplication.h"
#include "CommandBufferMgr.h"
#include "VulkanDrawable.h"
#include "meshdata.h"

VulkanRenderer::VulkanRenderer()
{
    vulkanSwapChain = new VulkanSwapChain(this);

    vulkanApplication = VulkanApplication::getInstance();
    vulkanDevice = &vulkanApplication->vulkanDevice;

    memset(&Depth, 0, sizeof(Depth));

    VulkanDrawable *drawableObj = new VulkanDrawable(this);
    drawableList.push_back(drawableObj);
}

void VulkanRenderer::initialize()
{
    createPresentationWindow();
    vulkanSwapChain->initialize();

    // we need a command buffers, so create a command buffer pool
    createCommandPool();

    // Let's create the swap chain color images and depth image
    buildSwapChainAndDepthImage();

    // Build the vertex buffer
    createVertexBuffer();

    const bool includeDepth = true;
    createRenderPass(includeDepth);

    createFrameBuffer(includeDepth);

    createShaders();

    createDescriptors();

    createPipelineStateManagement();

    createPushConstants();

    qDebug() << "done";

}

void VulkanRenderer::prepare()
{
    for (auto drawableObj : drawableList)
    {
        drawableObj->prepare();
    }
}


#ifdef _WIN32
// MS-Windows event handling function:
LRESULT CALLBACK VulkanRenderer::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    qDebug() << __func__;
    switch (uMsg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        for (auto drawableObj : VulkanApplication::getInstance()->vulkanRenderer->drawableList)
        {
            drawableObj->render();
        }

        return 0;
    default:
        break;
    }
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void VulkanRenderer::createPresentationWindow(const int& windowWidth, const int& windowHeight)
{
    assert(windowWidth > 0 || windowHeight > 0);
    width = windowWidth;
    height = windowHeight;

    WNDCLASSEX  winInfo;

    memset(&winInfo, 0, sizeof(WNDCLASSEX));
    // Initialize the window class structure:
    winInfo.cbSize			= sizeof(WNDCLASSEX);
    winInfo.style			= CS_HREDRAW | CS_VREDRAW;
    winInfo.lpfnWndProc		= WndProc;
    winInfo.cbClsExtra		= 0;
    winInfo.cbWndExtra		= 0;
    winInfo.hInstance		= connection; // hInstance
    winInfo.hIcon			= LoadIcon(nullptr, IDI_APPLICATION);
    winInfo.hCursor			= LoadCursor(nullptr, IDC_ARROW);
    winInfo.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
    winInfo.lpszMenuName	= nullptr;
    winInfo.lpszClassName	= name;
    winInfo.hIconSm			= LoadIcon(nullptr, IDI_WINLOGO);

    // Register window class:
    if (!RegisterClassEx(&winInfo)) {
        // It didn't work, so try to give a useful error:
        printf("Unexpected error trying to start the application!\n");
        fflush(stdout);
        exit(1);
    }

    // Create window with the registered class:
    RECT wr = { 0, 0, width, height };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    window = CreateWindowEx(0,
                            name,					// class name
                            name,					// app name
                            WS_OVERLAPPEDWINDOW |	// window style
                            WS_VISIBLE |
                            WS_SYSMENU,
                            100, 100,				// x/y coords
                            wr.right - wr.left,     // width
                            wr.bottom - wr.top,     // height
                            nullptr,					// handle to parent
                            nullptr,					// handle to menu
                            connection,				// hInstance
                            nullptr);					// no extra parameters

    if (!window) {
        // It didn't work, so try to give a useful error:
        printf("Cannot create a window in which to draw!\n");
        fflush(stdout);
        exit(1);
    }

    SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)(VulkanApplication::getInstance()));
    qDebug() << __func__;
}

void VulkanRenderer::render()
{
    qDebug() << __func__;
    MSG msg;
    bool isRunning = true;
    while (isRunning)
    {
        for (auto drawableObj : VulkanApplication::getInstance()->vulkanRenderer->drawableList)
        {
            drawableObj->update();
        }
        PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
        if (msg.message == WM_QUIT) {
            isRunning = false;
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            RedrawWindow(window, nullptr, nullptr, RDW_INTERNALPAINT);
        }
    }
}

#elif __linux__
/* Magic code to me until now. */

xcb_atom_t wmDeleteWin;
xcb_atom_t wmProtocols;
void VulkanRenderer::createPresentationWindow(const int &windowWidth, const int &windowHeight)
{
    qDebug() << __func__;
    assert(windowWidth > 0);
    assert(windowHeight > 0);
    width = windowWidth;
    height = windowHeight;

    const xcb_setup_t *setup;
    xcb_screen_iterator_t iter;
    int scr;

    connection = xcb_connect(NULL, &scr);
    if (connection == NULL) {
        std::cout << "Cannot find a compatible Vulkan ICD.\n";
        exit(-1);
    }

    setup = xcb_get_setup(connection);
    iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0)
        xcb_screen_next(&iter);

    screen = iter.data;
    //

    uint32_t value_mask, value_list[32];

    window = xcb_generate_id(connection);
    qDebug() << "window id: " << window;

    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = screen->black_pixel;
    value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE;

    xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0, width, height, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, value_mask, value_list);

//    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 1, 12, "WM_PROTOCOLS");
//    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, cookie, 0);

//    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(connection, 0, 16, "WM_DELETE_WINDOW");
//    reply = xcb_intern_atom_reply(connection, cookie2, 0);

//    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, (*reply).atom, 4, 32, 1, &(*reply).atom);
//    qDebug() << "---------";
//    free(reply);

    // try wmDelete
    xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
    xcb_intern_atom_cookie_t wmProtocolsCookie = xcb_intern_atom(connection, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *wmDeleteReply = xcb_intern_atom_reply(connection, wmDeleteCookie, nullptr);
    xcb_intern_atom_reply_t *wmProtocolsReply = xcb_intern_atom_reply(connection, wmProtocolsCookie, nullptr);
    wmDeleteWin = wmDeleteReply->atom;
    wmProtocols = wmProtocolsReply->atom;

    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, wmProtocolsReply->atom, 4, 32, 1, &wmDeleteReply->atom);


    xcb_map_window(connection, window);

    const uint32_t coords[] = { 100, 100 };
    xcb_configure_window(connection, window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
    xcb_flush(connection);

    qDebug() << "---------";
//    xcb_generic_event_t *e;
//    while((e = xcb_wait_for_event(connection))) {
//        if ((e->response_type & ~0x80) == XCB_EXPOSE)
//            break;
//    }
}

void VulkanRenderer::render()
{
    qDebug() << __func__;
    bool running = true;
    xcb_generic_event_t *event;
    xcb_client_message_event_t *cm;

    while (running)
    {
        event = xcb_wait_for_event(connection);

        qDebug() << event->response_type;
        switch (event->response_type & ~0x80) {
            case XCB_CLIENT_MESSAGE: {
                cm = (xcb_client_message_event_t *)event;
                if (cm->data.data32[0] == wmDeleteWin)
                    running = false;
                break;
            }
        }
        free(event);
        acquireBackBuffer();
        presentBackBuffer();
    }

    xcb_destroy_window(connection, window);
}

void VulkanRenderer::acquireBackBuffer()
{
    qDebug() << __func__;
    for (auto drawable : drawableList) {
        drawable->render();
    }
}

void VulkanRenderer::presentBackBuffer()
{
    qDebug() << __func__;
}

#endif // _WIN32

void VulkanRenderer::createCommandPool()
{
    VulkanDevice *vulkanDevice = &vulkanApplication->vulkanDevice;
    VkResult ret;

    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.pNext = nullptr;
    cmdPoolInfo.queueFamilyIndex = vulkanDevice->graphicsQueueIndex;
    cmdPoolInfo.flags = 0;

    ret = vkCreateCommandPool(vulkanDevice->vkDevice, &cmdPoolInfo, nullptr, &cmdPool);
    assert(ret == VK_SUCCESS);
}

void VulkanRenderer::buildSwapChainAndDepthImage()
{
    vulkanDevice->getDeviceQueue();

    vulkanSwapChain->createSwapChain(cmdDepthImage);

    createDepthImage();
}

void VulkanRenderer::createDepthImage()
{
    VkResult ret;
    bool pass;

    VkImageCreateInfo imageInfo = {};
    if (Depth.format == VK_FORMAT_UNDEFINED) {
        Depth.format = VK_FORMAT_D16_UNORM;
    }

    const VkFormat depthFormat = Depth.format;

    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(vulkanDevice->vkPhysicalDevice, depthFormat, &props);
    if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        qDebug() << "tiling optimal";
    } else if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
        qDebug() << "tiling linear";
    } else {
        qDebug() << "UnSupported Depth Format, try other Depth formats. \n";
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
    ret = vkCreateImage(vulkanDevice->vkDevice, &imageInfo, nullptr, &Depth.image);
    assert(ret == VK_SUCCESS);

    // Get the image memory requirements
    VkMemoryRequirements memRqrmnt;
    vkGetImageMemoryRequirements(vulkanDevice->vkDevice, Depth.image, &memRqrmnt);
    qDebug() << "memRqrmnt: size: " << memRqrmnt.size << " aligment: " << memRqrmnt.alignment <<" type bits: " << memRqrmnt.memoryTypeBits;

    VkMemoryAllocateInfo memAlloc = {};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.pNext = nullptr;
    memAlloc.allocationSize = 0;
    memAlloc.memoryTypeIndex = 0;
    memAlloc.allocationSize = memRqrmnt.size;

    // Determine the type of memory required with the help of memory properties
    pass = vulkanDevice->memoryTypeFromProperties(memRqrmnt.memoryTypeBits, 0, &memAlloc.memoryTypeIndex);
    assert(pass);

    ret = vkAllocateMemory(vulkanDevice->vkDevice, &memAlloc, nullptr, &Depth.mem);
    assert(ret == VK_SUCCESS);

    // Bind the allocated memory
    ret = vkBindImageMemory(vulkanDevice->vkDevice, Depth.image, Depth.mem, 0);
    assert(ret == VK_SUCCESS);

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

    if ( depthFormat == VK_FORMAT_D16_UNORM_S8_UINT ||
         depthFormat == VK_FORMAT_D24_UNORM_S8_UINT ||
         depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT )
    {
        imgViewInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    // Use command buffer to create the depth image.
    CommandBufferMgr::allocCommandBuffer(&vulkanDevice->vkDevice, cmdPool, &cmdDepthImage);
    CommandBufferMgr::beginCommandBuffer(cmdDepthImage);
    {
        setImageLayout(Depth.image, imgViewInfo.subresourceRange.aspectMask,
                       VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                       (VkAccessFlagBits)0, cmdDepthImage);
    }
    CommandBufferMgr::endCommandBuffer(cmdDepthImage);
    CommandBufferMgr::submitCommandBuffer(vulkanDevice->queue, &cmdDepthImage);

    imgViewInfo.image = Depth.image;
    ret = vkCreateImageView(vulkanDevice->vkDevice, &imgViewInfo, nullptr, &Depth.view);
    assert(ret == VK_SUCCESS);
}

void VulkanRenderer::setImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout,
                                    VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask, const VkCommandBuffer &cmdBuf)
{
    assert(cmdBuf != VK_NULL_HANDLE);

    assert(vulkanDevice->queue != VK_NULL_HANDLE);

    /* The image memory barrier is represented by the VkImageMemoryBarrier instance
     * and is applicable to the different memory access types via a specific image
     *  sub-resource range of the specified image object.
     */
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

    switch (newImageLayout) {
    // Ensure that anything that was copying from this image has completed
    // An image in this layout can only be used as the destination operand of the commands
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        imgMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;

    // Ensure any Copy or CPU writes to image are flushed
    // An image in this layout can only be used as a read-only shader resource
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        imgMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        imgMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;

    // An image in this layout can only be used as a framebuffer color attachment
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        imgMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        break;

    // An image in this layout can only be used as a framebuffer depth/stencil attachment
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        imgMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;
    default:
        break;
    }
    qDebug() << "imgMemoryBarrier.dstMask:" << imgMemoryBarrier.dstAccessMask;

    VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    vkCmdPipelineBarrier(cmdBuf, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &imgMemoryBarrier);
}

void VulkanRenderer::createVertexBuffer()
{
    CommandBufferMgr::allocCommandBuffer(&vulkanDevice->vkDevice, cmdPool, &cmdVertexBuffer);
    CommandBufferMgr::beginCommandBuffer(cmdVertexBuffer);

    for (auto drawableObj : drawableList) {
        qDebug() << "size of geometryData: " << sizeof(geometryData);
        drawableObj->createVertexBuffer(geometryData, sizeof(geometryData), sizeof(geometryData[0]), false);
    }

    CommandBufferMgr::endCommandBuffer(cmdVertexBuffer);
    CommandBufferMgr::submitCommandBuffer(vulkanDevice->queue, &cmdVertexBuffer);
}

void VulkanRenderer::createRenderPass(bool includeDepth, bool clear)
{
    qDebug() << __func__;
    VkResult ret;

    VkAttachmentDescription attachments[2];
    attachments[0].format = vulkanSwapChain->format;
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
        attachments[1].format = Depth.format;
        attachments[1].samples = NUM_SAMPLES;
        attachments[1].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments[1].flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
    }

    // Define the color buffer attachment binding point and layout information
    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Define the depth buffer attachment binding point and layout information
    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Specify the attachments - color, depth, resolve, preserve etc;
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

    // Sepcify the attachment and subpass associate with render pass
    VkRenderPassCreateInfo rpInfo = {};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpInfo.pNext = nullptr;
    rpInfo.attachmentCount = includeDepth ? 2 : 1;
    rpInfo.pAttachments = attachments;
    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses = &subpass;
    rpInfo.dependencyCount = 0;
    rpInfo.pDependencies = nullptr;

    // Create the render pass object
    ret = vkCreateRenderPass(vulkanDevice->vkDevice, &rpInfo, nullptr, &renderPass);
    assert(ret == VK_SUCCESS);
}

void VulkanRenderer::createFrameBuffer(bool includeDepth)
{
    qDebug() << __func__;
    VkResult ret;
    VkImageView attachments[2];
    attachments[1] = Depth.view;

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
    framebuffers.resize(vulkanSwapChain->swapChainImagesCount);
    for (i = 0; i < vulkanSwapChain->swapChainImagesCount; i++) {
        attachments[0] = vulkanSwapChain->colorBufferList[i].view;
        ret = vkCreateFramebuffer(vulkanDevice->vkDevice, &fbInfo, nullptr, &framebuffers.at(i));
        assert(ret == VK_SUCCESS);
    }
}

void *readFile(const char *spvFileName, size_t *fileSize)
{
    FILE *fp = fopen(spvFileName, "rb");
    assert(fp != nullptr);

    size_t retval;
    long int size;

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);

    void *spvShader = malloc(size+1);

    retval = fread(spvShader, size, 1, fp);
    assert(retval == 1);

    *fileSize = size;
    fclose(fp);

    return spvShader;

}

#ifdef _WIN32
#define SHADER_DIR(shaderName) "D:\\MyDisk\\RealityCreator\\src\\shader\\"#shaderName
#elif __linux__
#define SHADER_DIR(shaderName) "/home/huangwei/RealityCreator/src/shader/"#shaderName
#endif

void VulkanRenderer::createShaders()
{
    qDebug() << __func__;
    void *vertShaderCode, *fragShaderCode;
    size_t sizeVert, sizeFrag;

    vertShaderCode = readFile(SHADER_DIR(vert.spv), &sizeVert);
    fragShaderCode = readFile(SHADER_DIR(frag.spv), &sizeFrag);

    qDebug() << "sizeVert: " << sizeVert <<" , sizeFrag: " << sizeFrag;
    vulkanShader.buildShaderModuleWithSPV((uint32_t*)vertShaderCode, sizeVert, (uint32_t *)fragShaderCode, sizeFrag);
}

void VulkanRenderer::createDescriptors()
{
    for (auto drawableObj : drawableList) {
        drawableObj->createDescriptorSetLayout(false);
        drawableObj->createDescriptor(false);
    }
}

void VulkanRenderer::createPipelineStateManagement()
{
    for (auto drawableObj : drawableList) {
        drawableObj->createPipelineLayout();
    }

    qDebug() << __func__;
    vulkanPipeline.createPipelineCache();

    qDebug() <<"---";
    const VkBool32 depthPresent = VK_TRUE;
    for (auto vulkanDrawable : drawableList)
    {
       VkPipeline *pipeline = (VkPipeline*)malloc(sizeof(VkPipeline));
       if (vulkanPipeline.createPipeline(vulkanDrawable, pipeline, &vulkanShader, depthPresent)) {
           pipelineList.push_back(pipeline);
           vulkanDrawable->setPipeline(pipeline);
       } else {
            free(pipeline);
        }
    }
}

void VulkanRenderer::createPushConstants()
{
    CommandBufferMgr::allocCommandBuffer(&vulkanDevice->vkDevice, cmdPool, &cmdPushConstant);
    CommandBufferMgr::beginCommandBuffer(cmdPushConstant);

    enum ColorFlag {
        RED = 1,
        GREEN = 2,
        BLUE = 3,
        MIXED_COLOR = 4
    };

    float mixerValue = 0.3f;
    unsigned constColorRGBFlag = BLUE;

    unsigned pushConstants[2] = {};
    pushConstants[0] = constColorRGBFlag;
    memcpy(&pushConstants[1], &mixerValue, sizeof(float));

    // check if number of push constants does not exceed the allowed size
    int maxPushConstantSize = vulkanDevice->physicalDeviceProperties.limits.maxPushConstantsSize;
    if (sizeof(pushConstants) > maxPushConstantSize) {
        assert(0);
    }

    for (auto drawableObj : drawableList) {
        vkCmdPushConstants(cmdPushConstant, drawableObj->pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT,
                           0, sizeof(pushConstants), pushConstants);
    }

    CommandBufferMgr::endCommandBuffer(cmdPushConstant);
    CommandBufferMgr::submitCommandBuffer(vulkanDevice->queue, &cmdPushConstant);
}

































