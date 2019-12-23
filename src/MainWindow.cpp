#include "MainWindow.h"
#include "VulkanWindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(480, 640);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    vulkanWindow = new VulkanWindow;
    QWidget *wrapper = QWidget::createWindowContainer(vulkanWindow);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(wrapper,5);

    centralWidget->setLayout(layout);
}
