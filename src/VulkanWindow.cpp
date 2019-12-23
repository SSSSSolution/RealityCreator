#include "VulkanWindow.h"
#include <QVulkanFunctions>
#include <QApplication>
VulkanWindow::VulkanWindow(QWindow *parent)
    : QVulkanWindow(parent)
{
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

    devFuncs = vulkanWindow->vulkanInstance()
            ->deviceFunctions(vulkanWindow->device());
}

void VulkanRenderer::initSwapChainResources()
{
    qDebug("init swapchain resources");
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

    vulkanWindow->frameReady();
    vulkanWindow->requestUpdate();
}



























