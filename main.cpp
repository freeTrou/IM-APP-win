#include "mainwindow.h"
#include "loginwidget.h"
#include <QApplication>
#include"global.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    LoginWidget* l = new LoginWidget;
    myHelper::FormInCenter(l);//设置居中显示
    l->show();

    return a.exec();
}
