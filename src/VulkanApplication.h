#ifndef VULKANAPPLICATION_H
#define VULKANAPPLICATION_H

#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include <memory>
#include <mutex>

class VulkanRenderer;

class VulkanApplication
{
public:
    ~VulkanApplication();

    static VulkanApplication* getInstance();

    void initialize();
    void prepare();
    void update();
    bool render();
    void deInitialize();

private:
    VulkanApplication();

    VkResult createVulkanInstance(std::vector<const char*>& layers,
                                  std::vector<const char*>& extensions,
                                  const char *appNames);
    VkResult handShakeWithDevice(VkPhysicalDevice *gpu,
                                 std::vector<const char*> &layers,
                                 std::vector<const char *> &extensions);
    VkResult enumeratePhysicalDevices(std::vector<VkPhysicalDevice> &gpus);

public:
    VulkanInstance instanceObj;
    VulkanDevice *deviceObj;
    VulkanRenderer *rendererObj;

private:
    static std::unique_ptr<VulkanApplication> instance;
    static std::once_flag onlyOnce;

    bool debugFlag;
};

#endif // VULKANAPPLICATION_H
