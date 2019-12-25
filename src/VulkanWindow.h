#ifndef VULKANWIDGET_H
#define VULKANWIDGET_H

#include <QVulkanWindow>
#include <QKeyEvent>

class VulkanWindow : public QVulkanWindow
{
public:
    VulkanWindow(QWindow *parent = nullptr);
    ~VulkanWindow() {}

    QVulkanWindowRenderer *createRenderer() override;

protected:
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;
};

class VulkanRenderer : public QVulkanWindowRenderer
{
public:
    VulkanRenderer(QVulkanWindow *w);

    void initResources() override;
    void initSwapChainResources() override;
    void releaseSwapChainResources() override;
    void releaseResources() override;

    void startNextFrame() override;

private:
    void printPhysicalDeviceQueueFamilyProperties();
    void printPhysicalDeviceMemoryInformation();
    void printPhysicalDeviceProperties();

private:
    QVulkanWindow *vulkanWindow;
    QVulkanDeviceFunctions *devFuncs;
    QVulkanFunctions *vulkanFuncs;

    float green = 0;
    bool clear = true;

    VkPipelineCache pipelineCache;
    VkPipeline pipeline;
};

#endif // VULKANWIDGET_H
