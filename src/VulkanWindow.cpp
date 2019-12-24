#include "VulkanWindow.h"
#include <QVulkanFunctions>
#include <QApplication>
#include <QFile>
#include <QtGlobal>

VulkanWindow::VulkanWindow(QWindow *parent)
    : QVulkanWindow(parent)
{
    // 创建vulkan实例
    QVulkanInstance *inst = new QVulkanInstance;

#ifndef Q_OS_ANDROID
    inst->setLayers(QByteArrayList() << "VK_LAYER_LUNARG_standard_validation");
#else
    inst.setLayers(QByteArrayList()
                   << "VK_LAYER_GOOGLE_threading"
                   << "VK_LAYER_LUNARG_parameter_validation"
                   << "VK_LAYER_LUNARG_object_tracker"
                   << "VK_LAYER_LUNARG_core_validation"
                   << "VK_LAYER_LUNARG_image"
                   << "VK_LAYER_LUNARG_swapchain"
                   << "VK_LAYER_GOOGLE_unique_objects");
#endif

    if (!inst->create())
        qFatal("Failed to create Vulkan instance: %d", inst->errorCode());
    setVulkanInstance(inst);

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

// 打印物理设备信息
    QVector<VkPhysicalDeviceProperties> deviceProperties = availablePhysicalDevices();
    qDebug() << "physical device: ";
    for (const auto &dev : deviceProperties) {
        qDebug() << "\t" << dev.deviceName;
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

#if 0
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
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = blob1.size();
    shaderInfo.pCode = reinterpret_cast<const uint32_t *>(blob1.constData());
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
























