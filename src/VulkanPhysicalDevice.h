#ifndef VULKANPHYSICALDEVICE_H
#define VULKANPHYSICALDEVICE_H

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "VulkanDevice.h"
/* 物理设备(GPU)类
 */
class VulkanPhysicalDevice
{
public:
    static std::vector<VulkanPhysicalDevice> enumeratePhysicalDevices(VkInstance inst);

//    VkDevice createLogicalDevice();
    VkPhysicalDeviceProperties getProperties() const;
    std::string getGpuName() const;
    std::string getApiVersion() const;

private:
    VulkanPhysicalDevice(VkPhysicalDevice gpu);

private:
    VkPhysicalDevice m_gpu;
    /*
     * typedef struct VkPhysicalDeviceProperties {
     *   uint32_t                            apiVersion;
     *   uint32_t                            driverVersion;
     *   uint32_t                            vendorID;
     *   uint32_t                            deviceID;
     *   VkPhysicalDeviceType                deviceType;
     *   char                                deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
     *   uint8_t                             pipelineCacheUUID[VK_UUID_SIZE];
     *   VkPhysicalDeviceLimits              limits;
     *   VkPhysicalDeviceSparseProperties    sparseProperties;
     *   } VkPhysicalDeviceProperties;
    */
    VkPhysicalDeviceProperties m_props;
    /*
    typedef struct VkPhysicalDeviceMemoryProperties {
        uint32_t        memoryTypeCount;
        VkMemoryType    memoryTypes[VK_MAX_MEMORY_TYPES];
        uint32_t        memoryHeapCount;
        VkMemoryHeap    memoryHeaps[VK_MAX_MEMORY_HEAPS];
    } VkPhysicalDeviceMemoryProperties;
    */
    VkPhysicalDeviceMemoryProperties m_memProps;

};

#endif // VULKANPHYSICALDEVICE_H
