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
    QVulkanWindow *vulkanWindow;
    QVulkanDeviceFunctions *devFuncs;
    float green = 0;
};

#endif // VULKANWIDGET_H
