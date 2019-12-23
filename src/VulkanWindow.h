#ifndef VULKANWIDGET_H
#define VULKANWIDGET_H

#include <QVulkanWindow>

class VulkanWindow : public QVulkanWindow
{
public:
    VulkanWindow(QWindow *parent = nullptr);
    ~VulkanWindow() {}

    QVulkanWindowRenderer *createRenderer() override;
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
    QVulkanWindow *vulkanWindow;
    QVulkanDeviceFunctions *devFuncs;
    float green = 0;
};

#endif // VULKANWIDGET_H
