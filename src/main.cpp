#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();

    return app.exec();
    // 一般程序直接就退出了，
    // 在Qt框架里app.exec()进入事件循环，Qt主进程一直阻塞，等待输入
    // 就是初始化和显示完窗口后,程序进入一个while()循环，
    // 在这个循环里程序可以接受用户的键盘，鼠标的输入，vulkan也可以进行绘图。
    // 一般的，我们要通过重写父类的虚函数实现如何处理键盘和鼠标的输入，以及vulkan如何进行每一帧的绘图
    // 这些父类的虚函数已经在app.exec()里的某一处被调用了，我们通过重写这些函数来决定如何处理鼠标，键盘输入
}
