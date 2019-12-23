QT += core gui widgets

TARGET = reality_creator
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

INCLUDEPATH += /home/huangwei/study/Mirage/3thparty/1.1.121.1/x86_64/include
LIBS += -L/home/huangwei/study/Mirage/3thparty/1.1.121.1/x86_64/lib -lvulkan

SOURCES += \
    MainWindow.cpp \
    VulkanWindow.cpp \
    main.cpp

HEADERS += \
    MainWindow.h \
    VulkanWindow.h
