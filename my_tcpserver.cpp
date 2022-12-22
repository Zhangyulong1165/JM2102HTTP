#include "my_tcpserver.h"
#include "Logger.h"
#include "commheader.h"
#include <QCryptographicHash>
extern int listenPort;
extern QString vKey;
extern QMutex *m_pRecvMutex;
extern QByteArray recv_array;
my_tcpserver::my_tcpserver(QObject *parent) : QObject(parent)
{
    m_pTcpServer =new QTcpServer();
    m_pTcpSocket =new QTcpSocket();
    m_pTimer =new QTimer();
}
bool my_tcpserver::start_listen()
{
    if(m_pTcpServer->listen(QHostAddress::Any,listenPort))
    {
        qDebug()<<"start listen "<<listenPort;
        connect(m_pTcpServer,SIGNAL(newConnection()),this,SLOT(server_new_connection()));
        //m_pTcpSocket->setReadBufferSize(1024*48);
    }
    else {
        qDebug()<<"listen error";
    }
}
void my_tcpserver::server_new_connection()
{
    char logInfo[128]={0};
    m_pTcpSocket =m_pTcpServer->nextPendingConnection();
    if(m_pTcpSocket->isValid())
    {
        connect(m_pTcpSocket,SIGNAL(readyRead()),this,SLOT(server_read_data()));
    }
}
//处理接收数据，包头验证，MD5验证
void my_tcpserver::server_read_data()
{
    QByteArray buf =m_pTcpSocket->readAll();
    m_pRecvMutex->lock();
    recv_array.append(buf);
    m_pRecvMutex->unlock();
    return;
}

