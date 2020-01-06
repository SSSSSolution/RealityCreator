#ifndef VULKANAPPLICATION_H
#define VULKANAPPLICATION_H

#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanRenderer.h"
#include <memory>
#include <mutex>

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

public:
    VulkanInstance instanceObj;
    VulkanDevice *deviceObj;
    VulkanRenderer *rendererObj;

private:
    VulkanApplication();

    VkResult createVulkanInstance(std::vector<const char*>& layers,
                                  std::vector<const char*>& extensions,
                                  const char *applicationNames);
    VkResult handShakeWithDevice(VkPhysicalDevice *gpu,
                                 std::vector<const char*> &layers,
                                 std::vector<const char *> &extensions);
    VkResult enumeratePhysicalDevices(std::vector<VkPhysicalDevice> &gpus);

private:
    static std::unique_ptr<VulkanApplication> instance;
    static std::once_flag onlyOnce;

    bool debugFlag;
};

#endif // VULKANAPPLICATION_H
