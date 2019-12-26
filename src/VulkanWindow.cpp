#include "VulkanWindow.h"
#include <QVulkanFunctions>
#include <QApplication>
#include <QFile>
#include "VulkanLayersAndExtensions.h"
#include <QVulkanLayer>
#include "VulkanPhysicalDevice.h"
#include "VulkanInstance.h"
VulkanWindow::VulkanWindow(QWindow *parent)
    : QVulkanWindow(parent)
{
    VulkanInstance *instance = new VulkanInstance;;
    std::vector<const char *> l1 = {
        "VK_LAYER_LUNARG_standard_validation"
    };
    std::vector<const char *> l2 = {
        "VK_EXT_acquire_xlib_display",
        "VK_EXT_debug_report",
        "VK_EXT_direct_mode_display",
        "VK_EXT_display_surface_counter",
        "VK_KHR_display",
        "VK_KHR_get_physical_device_properties2",
        "VK_KHR_get_surface_capabilities2",
        "VK_KHR_surface",
        "VK_KHR_xcb_surface",
        "VK_KHR_xlib_surface",
        "VK_KHR_external_fence_capabilities",
        "VK_KHR_external_memory_capabilities",
        "VK_KHR_external_semaphore_capabilities",
        "VK_EXT_debug_utils"
    };
    instance->create(l1, l2);

    QVulkanInstance *inst = new QVulkanInstance;
    inst->setVkInstance(instance->getVkInstance());

    if (!inst->create()) {
        qApp->quit();
    }

    setVulkanInstance(inst);
    qDebug() << inst->supportedLayers();

    // test VulkanPhysicalDevice
    qDebug() << "GPU Info:";
    qDebug() << "============================";
    std::vector<VulkanPhysicalDevice> gpuList = VulkanPhysicalDevice::enumeratePhysicalDevices(inst->vkInstance());
    for (const auto &gpu : gpuList) {
        VkPhysicalDeviceProperties props = gpu.getProperties();
        qDebug() << "name: " << gpu.getGpuName().c_str();
        qDebug() << "api version: " << gpu.getApiVersion().c_str();
    }

    std::vector<const char *> layerList;
    std::vector<const char *> extensionList;
    VulkanDevice device = gpuList[0].createLogicalDevice(layerList, extensionList);

// 打印vulkan实例支持的的扩展
    QVulkanInfoVector<QVulkanExtension> extensions = inst->supportedExtensions();
    qDebug() << "extensions: ";
    for (const auto &extension : extensions) {
        qDebug() << "\t" << extension.name;
    }
// 打印vulkan实例支持的layer(层)
    QVulkanInfoVector<QVulkanLayer> layers = inst->supportedLayers();
    qDebug() << "layer: ";
    for (const auto &layer : layers) {
        qDebug() << "\t" << layer.name;
    }
}

QVulkanWindowRenderer *VulkanWindow::createRenderer()
{
    return new VulkanRenderer(this);
}

void VulkanWindow::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Escape) {
        qApp->quit();
    }
}

void VulkanWindow::keyReleaseEvent(QKeyEvent *ev)
{
    qDebug() << __func__ << ev->key();
}

VulkanRenderer::VulkanRenderer(QVulkanWindow *w)
    : vulkanWindow(w)
{
}

void VulkanRenderer::initResources()
{
    qDebug("init resources");

    // 获得vulkan的接口
    vulkanFuncs = vulkanWindow->vulkanInstance()->functions();
    devFuncs = vulkanWindow->vulkanInstance()
            ->deviceFunctions(vulkanWindow->device());

    printPhysicalDeviceQueueFamilyProperties();
    printPhysicalDeviceMemoryInformation();
    printPhysicalDeviceProperties();

    VulkanLayersAndExtensions layers(vulkanWindow->vulkanInstance(),
                                     reinterpret_cast<VkPhysicalDevice>(vulkanWindow->physicalDevice()));

#if 0
    // Vertex layout
    VkVertexInputBindingDescription vertexBindingDesc[] = {
        {
            0,
            8 * sizeof(float),
            VK_VERTEX_INPUT_RATE_VERTEX
        },
        {
            1,
            6 * sizeof(float),
            VK_VERTEX_INPUT_RATE_INSTANCE
        }
    };
    VkVertexInputAttributeDescription vertexAttrDesc[] = {
        {
            0,
            0,
            VK_FORMAT_R32G32B32_SFLOAT,
            0
        },
        {
            1,
            0,
            VK_FORMAT_R32G32B32_SFLOAT,
            5 * sizeof(float)
        },
        {
            2,
            1,
            VK_FORMAT_R32G32B32_SFLOAT,
            0
        },
        {
            3,
            1,
            VK_FORMAT_R32G32B32_SFLOAT,
            3 * sizeof(float)
        }
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;
    vertexInputInfo.flags = 0;
    vertexInputInfo.vertexBindingDescriptionCount = sizeof(vertexBindingDesc) / sizeof(vertexBindingDesc[0]);
    vertexInputInfo.pVertexBindingDescriptions = vertexBindingDesc;
    vertexInputInfo.vertexAttributeDescriptionCount = sizeof(vertexAttrDesc) / sizeof(vertexAttrDesc[0]);
    vertexInputInfo.pVertexAttributeDescriptions = vertexAttrDesc;
    // build vertex shader module
    VkShaderModule vtxShaderModule;
    QFile vtxFile(":/shader/vertex.vsh");
    if (!vtxFile.open(QIODevice::ReadOnly)) {
        qWarning("Failed to open :/shader/vertex.vsh");
        qApp->quit();
    }
    QByteArray blob = vtxFile.readAll();
    VkShaderModuleCreateInfo shaderInfo = {};
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = blob.size();
    shaderInfo.pCode = reinterpret_cast<const uint32_t *>(blob.constData());
    VkResult err = devFuncs->vkCreateShaderModule(vulkanWindow->device(),
                                                  &shaderInfo, nullptr,
                                                  &vtxShaderModule);
    if (err != VK_SUCCESS) {
        qWarning("failed to create shader module: %d", err);
    }

    // build fragment shader moduel
    VkShaderModule fragmentShaderModule;
    QFile fragFile(":/shader/fragment.frag");
    if (!fragFile.open(QIODevice::ReadOnly)) {
        qWarning("Failed to open :/shader/fragment.frag");
        qApp->quit();
    }
    QByteArray blob1 = vtxFile.readAll();
    VkShaderModuleCreateInfo shaderInfo1 = {};
    shaderInfo1.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo1.codeSize = blob1.size();
    shaderInfo1.pCode = reinterpret_cast<const uint32_t *>(blob1.constData());
    err = devFuncs->vkCreateShaderModule(vulkanWindow->device(),
                                          &shaderInfo1, nullptr,
                                          &fragmentShaderModule);
    if (err != VK_SUCCESS) {
        qWarning("failed to create shader module: %d", err);
    }

    // build layout
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSetLayoutBinding layoutBindings[] = {
        {
            0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            1,
            VK_SHADER_STAGE_VERTEX_BIT,
            nullptr
        },
        {
            1,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            1,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            nullptr
        }
    };
    VkDescriptorSetLayoutCreateInfo descLayoutInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        nullptr,
        0,
        sizeof(layoutBindings) / sizeof(layoutBindings[0]),
        layoutBindings
    };
    err = devFuncs->vkCreateDescriptorSetLayout(vulkanWindow->device(),
                                                &descLayoutInfo, nullptr,
                                                &descriptorSetLayout);
    if (err != VK_SUCCESS)
        qFatal("Failed to create descriptor set layout: %d", err);


    // build pipeline
    VkPipelineCacheCreateInfo pipelineCacheInfo;
    memset(&pipelineCacheInfo, 0, sizeof(pipelineCacheInfo));
    pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    err = devFuncs->vkCreatePipelineCache(vulkanWindow->device(), &pipelineCacheInfo,
                                          nullptr, &pipelineCache);
    if (err != VK_SUCCESS)
        qFatal("Failed to create pipeline cache: %d", err);

    VkGraphicsPipelineCreateInfo pipelineInfo;
    memset(&pipelineInfo, 0, sizeof(pipelineInfo));
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    VkPipelineShaderStageCreateInfo shaderStages[2] = {
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr,
            0,
            VK_SHADER_STAGE_VERTEX_BIT,
            vtxShaderModule,
            "main",
            nullptr
        },
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr,
            0,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            fragmentShaderModule,
            "main",
            nullptr
        }
    };
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;

    VkPipelineInputAssemblyStateCreateInfo ia;
    memset(&ia, 0, sizeof(ia));
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineInfo.pInputAssemblyState = &ia;

    VkPipelineViewportStateCreateInfo vp;
    memset(&vp, 0, sizeof(vp));
    vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.viewportCount = 1;
    vp.scissorCount = 1;
    pipelineInfo.pViewportState = &vp;

    VkPipelineRasterizationStateCreateInfo rs;
    memset(&rs, 0, sizeof(rs));
    rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.polygonMode = VK_POLYGON_MODE_FILL;
    rs.cullMode = VK_CULL_MODE_BACK_BIT;
    rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rs.lineWidth = 1.0f;
    pipelineInfo.pRasterizationState = &rs;

    VkPipelineMultisampleStateCreateInfo ms;
    memset(&ms, 0, sizeof(ms));
    ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms.rasterizationSamples = vulkanWindow->sampleCountFlagBits();
    pipelineInfo.pMultisampleState = &ms;

    VkPipelineDepthStencilStateCreateInfo ds;
    memset(&ds, 0, sizeof(ds));
    ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.depthTestEnable = VK_TRUE;
    ds.depthWriteEnable = VK_TRUE;
    ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    pipelineInfo.pDepthStencilState = &ds;

    VkPipelineColorBlendStateCreateInfo cb;
    memset(&cb, 0, sizeof(cb));
    cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    VkPipelineColorBlendAttachmentState att;
    memset(&att, 0, sizeof(att));
    att.colorWriteMask = 0xF;
    cb.attachmentCount = 1;
    cb.pAttachments = &att;
    pipelineInfo.pColorBlendState = &cb;

    VkDynamicState dynEnable[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dyn;
    memset(&dyn, 0, sizeof(dyn));
    dyn.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dyn.dynamicStateCount = sizeof(dynEnable) / sizeof(VkDynamicState);
    dyn.pDynamicStates = dynEnable;
    pipelineInfo.pDynamicState = &dyn;

//    pipelineInfo.layout = descriptorSetLayout;
    pipelineInfo.renderPass = vulkanWindow->defaultRenderPass();

    err = devFuncs->vkCreateGraphicsPipelines(vulkanWindow->device(),
                                              pipelineCache, 1, &pipelineInfo,
                                              nullptr, &pipeline);
    if (err != VK_SUCCESS)
        qFatal("Failed to create graphics pipeline: %d", err);
#endif
}

void VulkanRenderer::initSwapChainResources()
{
    qDebug("init swapchain resources");
    VkFormat colorFormat = vulkanWindow->colorFormat();
    qDebug() << "color format: " << colorFormat;
    qDebug() << "swapchain image size: " << vulkanWindow->swapChainImageSize();
    qDebug() << "window size: " << vulkanWindow->size();
    VkFormat depthFormat = vulkanWindow->depthStencilFormat();
    qDebug() << "depth format: " << depthFormat;

}

void VulkanRenderer::releaseSwapChainResources()
{
    qDebug("release swapchain resources");
}

void VulkanRenderer::releaseResources()
{
    qDebug("release resource");
}

void VulkanRenderer::startNextFrame()
{
//    static uint f = 0;
//    qDebug() << "frame " << f++;

#if 1
    green += 0.005f;
    if (green > 1.0f)
        green = 0.0f;

    VkClearColorValue clearColor = {{ 0.0f, green, 0.0f, 1.0f }};
    VkClearDepthStencilValue clearDS = { 1.0f, 0 };
    VkClearValue clearValues[2];
    memset(clearValues, 0, sizeof(clearValues));
    clearValues[0].color = clearColor;
    clearValues[1].depthStencil = clearDS;

    VkRenderPassBeginInfo rpBeginInfo;
    memset(&rpBeginInfo, 0, sizeof(rpBeginInfo));

    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = vulkanWindow->defaultRenderPass();
    rpBeginInfo.framebuffer = vulkanWindow->currentFramebuffer();
    const QSize sz = vulkanWindow->swapChainImageSize();
    rpBeginInfo.renderArea.extent.width = sz.width();
    rpBeginInfo.renderArea.extent.height = sz.height();
    rpBeginInfo.clearValueCount = 2;
    rpBeginInfo.pClearValues = clearValues;

    VkCommandBuffer cmdBuf = vulkanWindow->currentCommandBuffer();
    devFuncs->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    devFuncs->vkCmdEndRenderPass(cmdBuf);
#endif

#if 0
    VkCommandBuffer cb = vulkanWindow->currentCommandBuffer();
    const QSize sz = vulkanWindow->swapChainImageSize();

    VkClearColorValue clearColor = {{ 0.67f, 0.84f, 0.9f, 1.0f }};
    VkClearDepthStencilValue clearDS = { 1, 0 };
    VkClearValue clearValues[3];
    memset(clearValues, 0, sizeof(clearValues));
    clearValues[0].color = clearValues[2].color = clearColor;
    clearValues[1].depthStencil = clearDS;

    VkRenderPassBeginInfo rpBeginInfo;
    memset(&rpBeginInfo, 0, sizeof(rpBeginInfo));
    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = vulkanWindow->defaultRenderPass();
    rpBeginInfo.framebuffer = vulkanWindow->currentFramebuffer();
    rpBeginInfo.renderArea.extent.width = sz.width();
    rpBeginInfo.renderArea.extent.height = sz.height();
    rpBeginInfo.clearValueCount = vulkanWindow->sampleCountFlagBits() > VK_SAMPLE_COUNT_1_BIT ? 3 : 2;
    rpBeginInfo.pClearValues = clearValues;
    VkCommandBuffer cmdBuf = vulkanWindow->currentCommandBuffer();
    devFuncs->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
#endif



    vulkanWindow->frameReady();
    vulkanWindow->requestUpdate();
}

void VulkanRenderer::printPhysicalDeviceQueueFamilyProperties()
{
    uint32_t queueFamilyCount = 0;
    vulkanFuncs->vkGetPhysicalDeviceQueueFamilyProperties(
                vulkanWindow->physicalDevice(),
                &queueFamilyCount, nullptr);
    qDebug() << "queue family count: " << queueFamilyCount;
    QVector<VkQueueFamilyProperties> queueFamilyPropertiesList(queueFamilyCount);
    vulkanFuncs->vkGetPhysicalDeviceQueueFamilyProperties(
                vulkanWindow->physicalDevice(),
                &queueFamilyCount, queueFamilyPropertiesList.data());
    qDebug() << "queue family properties:";
    for (const auto &queueFamilyQueueProerties : queueFamilyPropertiesList) {
        qDebug() << "\tqueue flags: " << queueFamilyQueueProerties.queueFlags;
        qDebug() << "\tqueue count: " << queueFamilyQueueProerties.queueCount;
    }
}

void VulkanRenderer::printPhysicalDeviceMemoryInformation()
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vulkanFuncs->vkGetPhysicalDeviceMemoryProperties(
                vulkanWindow->physicalDevice(), &memProperties);
    qDebug() << "physical device memory infomation: ";
    qDebug() << "\tmemory type count: " << memProperties.memoryTypeCount;
    for (int i = 0; i < VK_MAX_MEMORY_TYPES; i++) {
        if (memProperties.memoryTypes[i].propertyFlags > 0 &&
            memProperties.memoryTypes[i].propertyFlags < 256) {
            qDebug() << "\t\tmemory type flags: " << memProperties.memoryTypes[i].propertyFlags;
            qDebug() << "\t\tmemory heap index: " << memProperties.memoryTypes[i].heapIndex;
        }
    }
    qDebug() << "\tmemory heap count: " << memProperties.memoryHeapCount;
    for (int i = 0; i < VK_MAX_MEMORY_HEAPS; i++) {
        if (memProperties.memoryHeaps[i].flags == 1 ||
            memProperties.memoryHeaps[i].flags == 2) {
            qDebug() << "\t\tmemory heap size: " << memProperties.memoryHeaps[i].size;
            qDebug() << "\t\tmemory heap flags: " << memProperties.memoryHeaps[i].flags;
        }
    }
}

void VulkanRenderer::printPhysicalDeviceProperties()
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vulkanFuncs->vkGetPhysicalDeviceProperties(
                vulkanWindow->physicalDevice(), &physicalDeviceProperties);
    qDebug() << "GPU properties:";
    qDebug() << "\tname: " << physicalDeviceProperties.deviceName;
    qDebug() << "\ttype: " << physicalDeviceProperties.deviceType;
    qDebug() << "\tvendor id: " << physicalDeviceProperties.vendorID;
}
























