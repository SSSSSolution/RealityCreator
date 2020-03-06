#include "VulkanLayersAndExtensions.h"
#include <QApplication>

#include "VulkanApplication.h"
#include "VulkanDevice.h"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

//    VulkanLayersAndExtensions layersAndExtensions;
//    layersAndExtensions.getInstanceLayerProperties();

    VulkanApplication * app = VulkanApplication::getInstance();
    app->initialize();
    app->prepare();
    app->render();
}
