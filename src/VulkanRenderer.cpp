#include "VulkanRenderer.h"
#include "VulkanSwapChain.h"
#include "VulkanApplication.h"
#include "CommandBufferMgr.h"

VulkanRenderer::VulkanRenderer()
{
    vulkanSwapChain = new VulkanSwapChain(this);

    vulkanApplication = VulkanApplication::getInstance();
    vulkanDevice = &vulkanApplication->vulkanDevice;

    memset(&Depth, 0, sizeof(Depth));
}

void VulkanRenderer::initialize()
{
    createPresentationWindow();
    vulkanSwapChain->initialize();

    // we need a command buffers, so create a command buffer pool
    createCommandPool();

    // Let's create the swap chain color images and depth image
    buildSwapChainAndDepthImage();


}

#ifdef _WIN32
// MS-Windows event handling function:
LRESULT CALLBACK VulkanRenderer::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    qDebug() << __func__;
//    switch (uMsg)
//    {
//    case WM_CLOSE:
//        PostQuitMessage(0);
//        break;
//    case WM_PAINT:
//        for each (VulkanDrawable* drawableObj in appObj->rendererObj->drawableList)
//        {
//            drawableObj->render();
//        }

//        return 0;
//    default:
//        break;
//    }
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void VulkanRenderer::createPresentationWindow(const int& windowWidth, const int& windowHeight)
{
    assert(windowWidth > 0 || windowHeight > 0);
    width = windowWidth;
    height = windowHeight;

    WNDCLASSEX  winInfo;

    name = L"Drawing Hello World";
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
}
#elif __linux__
/* Magic code to me until now. */
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

    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, cookie, 0);

    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(connection, 0, 16, "WM_DELETE_WINDOW");
    reply = xcb_intern_atom_reply(connection, cookie2, 0);

    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, (*reply).atom, 4, 32, 1, &(*reply).atom);
    qDebug() << "---------";
    free(reply);


    xcb_map_window(connection, window);

    const uint32_t coords[] = { 100, 100 };
    xcb_configure_window(connection, window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
    xcb_flush(connection);

    qDebug() << "---------";
    xcb_generic_event_t *e;
    while((e = xcb_wait_for_event(connection))) {
        if ((e->response_type & ~0x80) == XCB_EXPOSE)
            break;
    }
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
    } else if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
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


    VkMemoryAllocateInfo memAlloc = {};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.pNext = nullptr;
    memAlloc.allocationSize = 0;
    memAlloc.memoryTypeIndex = 0;
    memAlloc.allocationSize = memRqrmnt.size;
    qDebug() << "size: " << memRqrmnt.size;

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
    }

    VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    vkCmdPipelineBarrier(cmdBuf, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &imgMemoryBarrier);
}





























