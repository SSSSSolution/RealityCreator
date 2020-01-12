#include "VulkanRenderer.h"
#include "VulkanDevice.h"
#include <assert.h>
#include <string.h>
#include "Wrappers.h"

VulkanRenderer::VulkanRenderer(VulkanApplication *appObj, VulkanDevice *deviceObj)
    : appObj(appObj), deviceObj(deviceObj)
{
    memset(&depth, 0, sizeof(depth));
//    memset(&connection, 0, sizeof())

//    swapChainObj = new VulkanSwapChain(this);
    VulkanDrawable *drawableObj = new VulkanDrawable(this);
    drawalbeList.push_back(drawableObj);
}















