#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class VulkanWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() {}

private:
    VulkanWindow *vulkanWindow;
};

#endif // MAINWINDOW_H
