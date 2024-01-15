#include "clientsocket.h"
#include "databasemagr.h"
#include "myapp.h"

#include <QFile>
#include <QDebug>
#include <QHostAddress>
#include <QDataStream>
#include <QApplication>



//#define SERVER_IP "192.168.2.199"
//#define SERVER_PORT 60100

ClientSocket::ClientSocket(QObject *parent) :
    QObject(parent)
{
    m_nId = -1;

    m_tcpSocket = new QTcpSocket(this);

    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(SltReadyRead()));
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(SltConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(SltDisconnected()));
}

ClientSocket::~ClientSocket()
{
}

/**
 * 获取当前用户id
 */
int ClientSocket::GetUserId() const
{
    return  m_nId;
}

/**
 * 设置当前socket的用户id
 */
void ClientSocket::SetUserId(const int &id)
{
    m_nId = id;
}

/**
 * 检查是否连接
 */
void ClientSocket::CheckConnected()
{
    if (m_tcpSocket->state() != QTcpSocket::ConnectedState)
    {
        m_tcpSocket->connectToHost(MyApp::m_strHostAddr, MyApp::m_nMsgPort);
    }
}

/**
 * 主动断开连接
 */
void ClientSocket::ColseConnected()
{
    if (m_tcpSocket->isOpen()) {
        m_tcpSocket->abort();
    }
}

/**
 * 链接服务器接口
 * host
 * port
 */
void ClientSocket::ConnectToHost(const QString &host, const int &port)
{
    if (m_tcpSocket->isOpen()) m_tcpSocket->abort();
    m_tcpSocket->connectToHost(host, port);
}

/**
 * 链接服务器接口
 * host
 * port
 */
void ClientSocket::ConnectToHost(const QHostAddress &host, const int &port)
{
    if (m_tcpSocket->isOpen()) {
        m_tcpSocket->abort();
    }
    m_tcpSocket->connectToHost(host, port);
}

/**
 * tcp socket消息管理
 * type
 * dataVal
 */
void ClientSocket::SltSendMessage(const quint8 &type, const QJsonValue &dataVal)
{
    // 连接服务器
    if (!m_tcpSocket->isOpen()) {
        m_tcpSocket->connectToHost(MyApp::m_strHostAddr, MyApp::m_nMsgPort);
        m_tcpSocket->waitForConnected(1000);
    }
    // 超时1s后还是连接不上，直接返回
    if (!m_tcpSocket->isOpen()) {
        return;
    }

    // 构建 Json 对象
    QJsonObject json;
    json.insert("type", type);
    json.insert("from", m_nId);
    json.insert("data", dataVal);

    // 构建 Json 文档
    QJsonDocument document;
    document.setObject(json);
    qDebug() << "m_tcpSocket->write:" << document.toJson(QJsonDocument::Compact);
    m_tcpSocket->write(document.toJson(QJsonDocument::Compact));
}

/**
 *
 */
void ClientSocket::SltDisconnected()
{
    qDebug() << "has disconnecetd";
    m_tcpSocket->abort();
}

/**
 * @brief ClientSocket::SltConnected
 */
void ClientSocket::SltConnected()
{
    qDebug() << "has connecetd";
    emit signalStatus(ConnectedHost);
}


/**
 * 服务器消息处理槽函数
 */
void ClientSocket::SltReadyRead()
{
    // 读取socket数据
       QByteArray byRead = m_tcpSocket->readAll();
       QJsonParseError jsonError;
       // 转化为 JSON 文档
       QJsonDocument doucment = QJsonDocument::fromJson(byRead, &jsonError);
       // 解析未发生错误
       if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
       {
           // JSON 文档为对象
           if (doucment.isObject())
           {
               // 转化为对象
               QJsonObject jsonObj = doucment.object();
               QJsonValue dataVal = jsonObj.value("data");

               int nType = jsonObj.value("type").toInt();

               // 根据消息类型解析服务器消息
               switch(nType)
               {
                   case Register: // 用户注册

                   break;
                   case Login:  // 用户登录
                        ParseLogin(dataVal);//用户登录处理函数
                   break;
               }

            }
       }
}

//用户登录处理
void ClientSocket::ParseLogin(const QJsonValue &dataVal)
{
    QJsonObject dataObj = dataVal.toObject();
    int id = dataObj.value("id").toInt();
    QString msg = dataObj.value("msg").toString();
    if(id > 0 && msg == "ok")//登录成功
    {
        this->m_nId = id;
        emit signalStatus(LoginSuccess);//发送登录成功信号
        qDebug()<<"登录成功" << id;
    }
    else if(id == -1)
    {
        qDebug()<<"用户未注册" << id;
    }
    else if(id == -2)
    {
        emit signalStatus(LoginRepeat);
        qDebug()<<"用户已在线" << id;
    }
    else
    {
        qDebug()<<"特殊情况" << id << " " <<msg;
    }
}
