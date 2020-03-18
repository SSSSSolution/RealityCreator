QT += core gui widgets

TARGET = reality_creator
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

# 支持c++11标准
CONFIG += c++11 exceptions


INCLUDEPATH += ../3thparty/include
# linux 环境:`
unix {
#如何配置安装vulkan请参 https://vulkan.lunarg.com/doc/sdk/1.1.121.1/linux/getting_started.html，
#将头文件和库文件目录配置好即可
# vulkan头文件所在目录
INCLUDEPATH += /home/huangwei/study/Mirage/3thparty/1.1.121.1/x86_64/include
# vulkan库文件所在目录
LIBS += -L/home/huangwei/study/Mirage/3thparty/1.1.121.1/x86_64/lib -lvulkan
LIBS += -lxcb
}
# windows 环境:
win32 {
#如何配置安装vulkan请参考　https://vulkan.lunarg.com/doc/sdk/latest/windows/getting_started.html
INCLUDEPATH += C:\VulkanSDK\1.1.130.0\Include
LIBS += -LC:\VulkanSDK\1.1.130.0\Lib -lvulkan-1
LIBS += -lgdi32 -lkernel32 -luser32
}

SOURCES += \
    CommandBufferMgr.cpp \
    VulkanApplication.cpp \
    VulkanDescriptor.cpp \
    VulkanDevice.cpp \
    VulkanDrawable.cpp \
    VulkanInstance.cpp \
    VulkanLayersAndExtensions.cpp \
    VulkanPipeline.cpp \
    VulkanRenderer.cpp \
    VulkanShader.cpp \
    VulkanSwapChain.cpp \
    main.cpp

HEADERS += \
    CommandBufferMgr.h \
    VulkanApplication.h \
    VulkanDescriptor.h \
    VulkanDevice.h \
    VulkanDrawable.h \
    VulkanInstance.h \
    VulkanLayersAndExtensions.h \
    VulkanPipeline.h \
    VulkanRenderer.h \
    VulkanShader.h \
    VulkanSwapChain.h \
    headers.h \
    meshdata.h

RESOURCES += \
    shaders.qrc

DISTFILES +=
