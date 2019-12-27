#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#include <vulkan/vulkan.h>
#include <vector>

class VulkanInstance
{
public:
    VulkanInstance();
    ~VulkanInstance() {}
    VkResult create(std::vector<const char *> &layers,
                    std::vector<const char *> &extensions);
    VkInstance getVkInstance();
    VkResult createDebugReportCallback();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugFunction(VkFlags msgFlags,
                                                        VkDebugReportObjectTypeEXT objType,
                                                        uint64_t srcObject,
                                                        size_t location,
                                                        int32_t msgCode,
                                                        const char *layerPrefix,
                                                        const char *msg,
                                                        void *userData);


private:
    VkInstance instance;
    VkDebugReportCallbackCreateInfoEXT dbgReportInfo = {};
    VkDebugReportCallbackEXT m_debugReportCallback;
};

#endif // VULKANINSTANCE_H
