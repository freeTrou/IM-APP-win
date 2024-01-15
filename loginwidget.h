#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QFile>
#include "clientsocket.h"

namespace Ui {
class LoginWidget;
}

/*
登录界面对应类
*/
class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();
private:
    void mousePressEvent(QMouseEvent* event) override; //鼠标点击事件
    void mouseMoveEvent(QMouseEvent *event) override; //鼠标移动事件

private slots:
    void on_btnLogin_clicked();
    void on_btnWinMenu_clicked();
    void on_btnCancel_clicked();
    void on_btnWinClose_clicked();
    void on_btnWinMin_clicked();
    void on_btnRegister_clicked();
    void on_btnCancel2_clicked();


    void onSignalMessage(const quint8 &type, const QJsonValue &dataVal);//响应发送消息信号槽函数
    void onSignalStatus(const quint8 &state);//响应发送状态信号槽函数

private:
    Ui::LoginWidget *ui;

    QPoint diff_pos;  // 鼠标和窗口的相对位移
    QPoint window_pos;
    QPoint mouse_pos;

    ClientSocket* m_TcpSocket;
};

#endif // LOGINWIDGET_H
