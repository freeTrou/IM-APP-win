#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QMenu>
#include <QAction>
#include <QSystemTrayIcon>
#include "clientsocket.h"

namespace Ui {
class MainWindow;
}

/*
主界面对应类
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void mousePressEvent(QMouseEvent* event) override; //鼠标点击事件
    void mouseMoveEvent(QMouseEvent *event) override; //鼠标移动事件

    void setSocket(ClientSocket* tcpSocket,const QString& name);

private:
    void InitSysTrayIcon();//初始化托盘菜单

private slots:
    void SltMainPageChanged(int index);
    void on_btnWinClose_clicked();

    // 托盘菜单处理
    void SltTrayIcoClicked(QSystemTrayIcon::ActivationReason reason);
    void SltTrayIconMenuClicked(QAction *action);

    void SltTcpStatus(const quint8 &state);//响应发送状态信号槽函数
    void SltTcpReply(const quint8 &type, const QJsonValue &dataVal);//响应发送消息信号槽函数

    void SltQuitApp();//程序退出处理

private:
    Ui::MainWindow *ui;

    // 按钮切换界面
    QButtonGroup* m_btnGroup;

    QPoint diff_pos;  // 鼠标和窗口的相对位移
    QPoint window_pos;
    QPoint mouse_pos;

    QSystemTrayIcon* systemTrayIcon; //托盘

    ClientSocket* m_TcpSocket;
};

#endif // MAINWINDOW_H
