#include "loginwidget.h"
#include "ui_loginwidget.h"
#include "mainwindow.h"
#include "myapp.h"
#include "global.h"
#include <QMouseEvent>
#include <QJsonObject>
#include <QJsonDocument>


LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/resource/background/app.png"));
    ui->stackedWidget->setCurrentIndex(0);

    //去掉Qt自带的标题栏
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);
    //去掉黑边，使透明生效
    this->setAttribute(Qt::WA_TranslucentBackground);

    /*
    QFile file(":/qss/resource/qss/default.css");
    file.open(QIODevice::ReadOnly);
    //设置样式表
    qApp->setStyleSheet(file.readAll());//全局样式表
    file.close();*/
    myHelper::setStyle("default");


    //给登录输入框添加图片
    ui->lineEditUser->SetIcon(QPixmap(":/resource/common/ic_user.png"));
    ui->lineEditPasswd->SetIcon(QPixmap(":/resource/common/ic_lock.png"));

    //初始化套接字
    m_TcpSocket = new ClientSocket;
    m_TcpSocket->CheckConnected();//检查是否连接

    ui->labelWinTitle->setText("开始登录");

    //关联信号槽
    connect(m_TcpSocket,&ClientSocket::signalStatus,this,&LoginWidget::onSignalStatus);
    connect(m_TcpSocket,&ClientSocket::signalMessage,this,&LoginWidget::onSignalMessage);
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

//无边框窗口的移动
void LoginWidget::mousePressEvent(QMouseEvent* event)
{
    mouse_pos = event->globalPos();
    window_pos = this->pos();
    diff_pos = mouse_pos - window_pos;
}

void LoginWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pos = event->globalPos();
    //this->move(pos);
    this->move(pos - diff_pos);
}

/*
 *按钮对应槽函数
*/

//设置按钮
void LoginWidget::on_btnWinMenu_clicked()
{
    //ui->stackedWidget-
    ui->stackedWidget->setCurrentIndex(1);
}

//第二页 取消按钮
void LoginWidget::on_btnCancel_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

//登录按钮
void LoginWidget::on_btnLogin_clicked()
{
    //检查是否连接
    m_TcpSocket->CheckConnected();

    QString username = ui->lineEditUser->text();
    QString passwd = ui->lineEditPasswd->text();

    // 构建 Json 对象
    QJsonObject json;
    json.insert("name", username);
    json.insert("passwd", passwd);

    //发送json数据包
    m_TcpSocket->SltSendMessage(0x11,json);


    //显示主窗口
    MainWindow* mainWindow = new MainWindow;
    mainWindow->show();

    //隐藏登录窗口
    this->hide();


}

//关闭按钮
void LoginWidget::on_btnWinClose_clicked()
{
    this->close();
}

//最小化按钮
void LoginWidget::on_btnWinMin_clicked()
{
    this->showMinimized();
}

void LoginWidget::on_btnRegister_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void LoginWidget::on_btnCancel2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

//响应发送消息信号槽函数
void LoginWidget::onSignalMessage(const quint8 &type, const QJsonValue &dataVal)
{

}

//响应发送状态信号槽函数
void LoginWidget::onSignalStatus(const quint8 &state)
{
    switch (state)
    {
        case LoginRepeat: //重复登录
            ui->labelWinTitle->setText("用户已在线");
        break;
        case LoginSuccess: //登录成功
        {
            ui->labelWinTitle->setText("登录成功");
            //登录成功后取消这个信号槽的关联
            disconnect(m_TcpSocket,&ClientSocket::signalStatus,this,&LoginWidget::onSignalStatus);

            //显示主窗口
            MainWindow* mainWindow = new MainWindow;
            mainWindow->show();

            mainWindow->setSocket(m_TcpSocket,ui->lineEditUser->text());//将socket和用户名传入主窗口

            //隐藏登录窗口
            this->hide();
        }
        break;
        case LoginPasswdError: //登录失败
            ui->labelWinTitle->setText("登录失败");
        break;
    }
}
