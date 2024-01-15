#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "animationstackedwidget.h"
#include <QMouseEvent>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //去掉Qt自带的标题栏
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);

    m_btnGroup = new QButtonGroup(this);
    m_btnGroup->addButton(ui->btnFrind, 0);
    m_btnGroup->addButton(ui->btnGroup, 1);
    m_btnGroup->addButton(ui->btnConversation, 2);
    m_btnGroup->addButton(ui->btnApplay, 3);

    connect(m_btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(SltMainPageChanged(int)));

    InitSysTrayIcon();//托盘菜单初始化
}

MainWindow::~MainWindow()
{
    delete ui;
}


//无边框窗口的移动
void MainWindow::mousePressEvent(QMouseEvent* event)
{
    mouse_pos = event->globalPos();
    window_pos = this->pos();
    diff_pos = mouse_pos - window_pos;
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pos = event->globalPos();
    //this->move(pos);
    this->move(pos - diff_pos);
}

void MainWindow::setSocket(ClientSocket *tcpSocket, const QString &name)
{
    if(tcpSocket != nullptr)
    {
        this->m_TcpSocket = tcpSocket;
        connect(m_TcpSocket,&ClientSocket::signalStatus,this,&MainWindow::SltTcpStatus);
        connect(m_TcpSocket,&ClientSocket::signalMessage,this,&MainWindow::SltTcpReply);
    }
}

/**
 * 托盘菜单初始化
 */
void MainWindow::InitSysTrayIcon()
{
    systemTrayIcon = new QSystemTrayIcon(this);
    systemTrayIcon->setIcon(QIcon(":/resource/background/app.png"));

    QMenu *m_trayMenu = new QMenu(this);
    m_trayMenu->addAction("我在线上");
    m_trayMenu->addAction("离线");
    m_trayMenu->addSeparator();
    m_trayMenu->addAction("显示主面板");
    m_trayMenu->addSeparator();
    m_trayMenu->addAction("退出");

    systemTrayIcon->setContextMenu(m_trayMenu);
    systemTrayIcon->show();

    connect(systemTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this, SLOT(SltTrayIcoClicked(QSystemTrayIcon::ActivationReason)));
    connect(m_trayMenu, SIGNAL(triggered(QAction*)), this, SLOT(SltTrayIconMenuClicked(QAction*)));
}

//槽函数

//主界面按钮切换
void MainWindow::SltMainPageChanged(int index)
{
    static int s_nIndex = 0;
    if (index == s_nIndex) return;

    ui->GCStackedWidget->setLength(ui->GCStackedWidget->width(),
                                   index < s_nIndex
                                   ? AnimationStackedWidget::LeftToRight
                                   : AnimationStackedWidget::RightToLeft);

    ui->GCStackedWidget->start(index);
    s_nIndex = index;
}

/**
 * @brief MainWindow::SltTrayIcoClicked
 * 托盘菜单
 * @param reason
 */
void MainWindow::SltTrayIcoClicked(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    {
    }
        break;
    case QSystemTrayIcon::DoubleClick://双击
    {
        if (!this->isVisible())
            this->show();
    }
        break;
    default:
        break;
    }
}

// 托盘菜单
void MainWindow::SltTrayIconMenuClicked(QAction *action)
{
    if ("退出" == action->text()) {
        //m_bQuit = true;
        //m_TcpSocket->SltSendOffline();
        this->hide();
        QTimer::singleShot(500, this, SLOT(SltQuitApp()));
    }
    else if ("显示主面板" == action->text()) {
        this->show();
    }
    else if (!QString::compare("我在线上", action->text()))
    {
        //m_TcpSocket->CheckConnected();
        qDebug()<<"我在线上按钮";
    }
    else if (!QString::compare("离线", action->text()))
    {
        //m_TcpSocket->ColseConnected();
        qDebug()<<"离线按钮";
    }
}

void MainWindow::SltTcpStatus(const quint8 &state)
{

}

void MainWindow::SltTcpReply(const quint8 &type, const QJsonValue &dataVal)
{

}

/**
 * @brief MainWindow::SltQuitApp
 * 程序退出处理
 */
void MainWindow::SltQuitApp()
{
    // 关闭socket
    delete m_TcpSocket;
    m_TcpSocket = nullptr;

    delete ui;
    qApp->quit();
}

/*按钮槽函数*/
void MainWindow::on_btnWinClose_clicked()
{
    this->hide();//当前窗口隐藏
}
